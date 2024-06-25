#include "stypes.h"
#include "setjmp.h"
#include "runtime.h"
#include "string.h"

jmp_buf entry;
SVALUE saved_call_f;
SVALUE saved_call_args[128];
size_t saved_call_len;

size_t GC_DEBUG_MODE;
// minor gc variables
void *C_STACK_BOTTOM;
SVALUE **GC_MUTATION_LIST;
size_t GC_MUTATION_LIST_LEN;
size_t GC_MUTATION_LIST_SIZE;

// major gc variables

size_t GC_HEAP_SIZE;
void *GC_HEAP_FROMSPACE;
void *GC_HEAP_FROMSPACE_LIMIT;
void *GC_HEAP_FROMSPACE_ALLOC_PTR;

void *GC_HEAP_TOSPACE;
void *GC_HEAP_TOSPACE_LIMIT;
void *GC_HEAP_TOSPACE_ALLOC_PTR;

void gc_summary()
{
    if (GC_DEBUG_MODE)
    {
        printf("=======GC SUMMARY=========\n");
        printf("GC_MUTATION_LIST_LEN: %llu\n", GC_MUTATION_LIST_LEN);
        printf("GC_HEAP_FROMSPACE: %llu\n", GC_HEAP_FROMSPACE);
        printf("GC_HEAP_FROMSPACE_LIMIT: %llu\n", GC_HEAP_FROMSPACE_LIMIT);
        printf("GC_HEAP_FROMSPACE_ALLOC_PTR: %llu\n", GC_HEAP_FROMSPACE_ALLOC_PTR);
        printf("\n");

        printf("GC_HEAP_TOSPACE: %llu\n", GC_HEAP_TOSPACE);
        printf("GC_HEAP_TOSPACE_LIMIT: %llu\n", GC_HEAP_TOSPACE_LIMIT);
        printf("GC_HEAP_TOSPACE_ALLOC_PTR: %llu\n", GC_HEAP_TOSPACE_ALLOC_PTR);
    }
}

void gc_track_mutation(SVALUE *obj)
{
    if ((void *)obj >= GC_HEAP_FROMSPACE &&
        (void *)obj < GC_HEAP_FROMSPACE_LIMIT)
    {
        if (GC_MUTATION_LIST_LEN + 1 > GC_MUTATION_LIST_SIZE)
        {
            fprintf(stderr, "gc_track_mutation: too many mutations\n");
            fprintf(stderr, "TODO: consider resize GC_MUTATION_LIST\n");
            exit(-1);
        }
        GC_MUTATION_LIST[GC_MUTATION_LIST_LEN] = obj;
        GC_MUTATION_LIST_LEN++;
    }
}

void cheney_scan(SVALUE copy(SVALUE obj), void *start, void **end)
{
    void *scan_ptr = start;
    while (scan_ptr < *end)
    {

        SHEADER *obj = (SHEADER *)scan_ptr;
        switch (obj->type)
        {
        case STYPE_VECTOR:
            size_t vec_len = ((SVECTOR *)scan_ptr)->len;
            SVALUE *vec = (SVALUE *)(scan_ptr + sizeof(SVECTOR));
            for (size_t i = 0; i < vec_len; i++)
            {
                vec[i] = copy(vec[i]);
            }
            break;
        case STYPE_PROCEDURE:
            SPROCEDURE *proc = (SPROCEDURE *)scan_ptr;
            proc->env = copy(proc->env);
            break;
        case STYPE_STRING:
            break;
        case STYPE_PAIR:
            SPAIR *pair = (SPAIR *)scan_ptr;
            pair->car = copy(pair->car);
            pair->cdr = copy(pair->cdr);
            break;
        default:
            fprintf(stderr, "cheney scan: unrecognized type: %llu\n", obj->type);
            exit(-1);
        }
        scan_ptr += obj->len;
    }
}

SVALUE minor_gc_copy(SVALUE object_reference)
{

    if ((void *)object_reference == NULL || SVALUE_TYPE(object_reference) != STYPE_POINTER)
    {
        return object_reference;
    }

    SHEADER *header = (SHEADER *)object_reference;

    if (header->forward)
    {
        return (SVALUE)(header->forward);
    }

    if ((void *)object_reference >= GC_HEAP_FROMSPACE &&
        (void *)object_reference < GC_HEAP_FROMSPACE_LIMIT)
    {
        return object_reference;
    }

    if (GC_HEAP_FROMSPACE_ALLOC_PTR + header->len > GC_HEAP_FROMSPACE_LIMIT)
    {
        major_gc();
    }

    memcpy(GC_HEAP_FROMSPACE_ALLOC_PTR, (void *)object_reference, header->len);
    header->forward = GC_HEAP_FROMSPACE_ALLOC_PTR;

    GC_HEAP_FROMSPACE_ALLOC_PTR += header->len;
    return (SVALUE)(header->forward);
}

void minor_gc()
{
    if (GC_DEBUG_MODE)
    {
        printf("minor gc starting ...\n");
        gc_summary();
    }

    void *scan_start = GC_HEAP_FROMSPACE_ALLOC_PTR;
    saved_call_f = minor_gc_copy(saved_call_f);

    for (size_t i = 0; i < saved_call_len; i++)
    {
        saved_call_args[i] = minor_gc_copy(saved_call_args[i]);
    }

    for (size_t i = 0; i < GC_MUTATION_LIST_LEN; i++)
    {
        *(GC_MUTATION_LIST[i]) = minor_gc_copy(*(GC_MUTATION_LIST[i]));
    }
    GC_MUTATION_LIST_LEN = 0;

    cheney_scan(minor_gc_copy, scan_start, &GC_HEAP_FROMSPACE_ALLOC_PTR);

    if (GC_DEBUG_MODE)
    {
        printf("minor gc finished.\n");
        gc_summary();
    }
    // trampoline
    longjmp(entry, 0);
}

SVALUE major_gc_copy(SVALUE object_reference)
{
    if ((void *)object_reference == NULL || SVALUE_TYPE(object_reference) != STYPE_POINTER)
    {
        return object_reference;
    }
    SHEADER *header = (SHEADER *)object_reference;
    if ((void *)header->forward >= GC_HEAP_TOSPACE &&
        (void *)header->forward < GC_HEAP_TOSPACE_LIMIT)
    {
        return (SVALUE)(header->forward);
    }
    else
    {
        if (GC_HEAP_TOSPACE_ALLOC_PTR + header->len > GC_HEAP_TOSPACE_LIMIT)
        {
            fprintf(stderr, "major_gc_copy: insufficient memory\n");
            fprintf(stderr, "TODO: consider resizing heap\n");
            exit(-1);
        }
        memcpy(GC_HEAP_TOSPACE_ALLOC_PTR, (void *)object_reference, header->len);

        if (header->forward)
        {
            // this object has both a stack version and a fromspace version
            SHEADER *stack_version = header;
            SHEADER *fromspace_version = header->forward;
            stack_version->forward = GC_HEAP_TOSPACE_ALLOC_PTR;
            fromspace_version->forward = GC_HEAP_TOSPACE_ALLOC_PTR;

            // cleanup forward pointer
            ((SHEADER *)GC_HEAP_TOSPACE_ALLOC_PTR)->forward = NULL;
        }

        GC_HEAP_TOSPACE_ALLOC_PTR += header->len;
        return (SVALUE)(header->forward);
    }
}

void swap(void **a, void **b)
{
    void *t = *a;
    *a = *b;
    *b = t;
}

void major_gc()
{
    if (GC_DEBUG_MODE)
    {
        printf("major gc starting...\n");
        gc_summary();
    }

    void *scan_start = GC_HEAP_TOSPACE_ALLOC_PTR;
    saved_call_f = major_gc_copy(saved_call_f);
    for (size_t i = 0; i < saved_call_len; i++)
    {
        saved_call_args[i] = major_gc_copy(saved_call_args[i]);
    }
    cheney_scan(major_gc_copy, scan_start, &GC_HEAP_TOSPACE_ALLOC_PTR);

    // cleanups
    GC_HEAP_FROMSPACE_ALLOC_PTR = GC_HEAP_FROMSPACE;
    swap(&GC_HEAP_FROMSPACE, &GC_HEAP_TOSPACE);
    swap(&GC_HEAP_FROMSPACE_LIMIT, &GC_HEAP_TOSPACE_LIMIT);
    swap(&GC_HEAP_FROMSPACE_ALLOC_PTR, &GC_HEAP_TOSPACE_ALLOC_PTR);

    if (GC_DEBUG_MODE)
    {
        printf("major gc finished.\n");
        gc_summary();
    }
    // trampoline
    longjmp(entry, 0);
}
