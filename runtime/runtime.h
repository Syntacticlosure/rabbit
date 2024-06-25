#ifndef RUNTIME
#define RUNTIME
#include "stypes.h"
#include "setjmp.h"
#include "fcall.h"

extern jmp_buf entry;
extern SVALUE saved_call_f;
extern SVALUE saved_call_args[128];
extern size_t saved_call_len;



extern size_t GC_DEBUG_MODE;
// minor gc variables
extern void* C_STACK_BOTTOM;
#define C_STACK_POINTER alloca(0) // doesn't work on LLVM and TCC
#define GC_STACK_SIZE (1024*1024)
#define GC_STACK_LIMIT (900*1024)
#define GC_STACK_CHECK (C_STACK_BOTTOM - C_STACK_POINTER > GC_STACK_LIMIT)
extern SVALUE** GC_MUTATION_LIST;
extern size_t GC_MUTATION_LIST_LEN;
extern size_t GC_MUTATION_LIST_SIZE;


// cheney's garbage collector
void cheney_scan(SVALUE copy(SVALUE obj), void* start,void** end);
void gc_track_mutation(SVALUE* obj);

// minor gc functions
void minor_gc();
SVALUE minor_gc_copy(SVALUE object_reference);

// major gc variables
extern size_t GC_HEAP_SIZE;
extern void* GC_HEAP_FROMSPACE;
extern void* GC_HEAP_FROMSPACE_LIMIT;
extern void* GC_HEAP_FROMSPACE_ALLOC_PTR;

extern void* GC_HEAP_TOSPACE;
extern void* GC_HEAP_TOSPACE_LIMIT;
extern void* GC_HEAP_TOSPACE_ALLOC_PTR;

// major gc functions
void major_gc();
SVALUE major_gc_copy(SVALUE object_reference);

#define GC_SETUP C_STACK_BOTTOM = C_STACK_POINTER; \
                 GC_DEBUG_MODE = 0; \
                 GC_MUTATION_LIST_SIZE = 4096; \
                 GC_MUTATION_LIST = malloc(sizeof(SVALUE)* GC_MUTATION_LIST_SIZE); \
                 GC_MUTATION_LIST_LEN = 0; \
                 \
                 GC_HEAP_SIZE = 16 * 1024 * 1024; \
                 GC_HEAP_FROMSPACE = malloc(GC_HEAP_SIZE*2); \
                 GC_HEAP_FROMSPACE_LIMIT = GC_HEAP_FROMSPACE + GC_HEAP_SIZE; \
                 GC_HEAP_FROMSPACE_ALLOC_PTR = GC_HEAP_FROMSPACE; \
                 \
                 GC_HEAP_TOSPACE = GC_HEAP_FROMSPACE_LIMIT; \
                 GC_HEAP_TOSPACE_LIMIT = GC_HEAP_TOSPACE + GC_HEAP_SIZE; \
                 GC_HEAP_TOSPACE_ALLOC_PTR = GC_HEAP_TOSPACE; \


#define SETUP GC_SETUP \
              pSPROCEDURE topcont = alloca(sizeof(SPROCEDURE)); \
                topcont->header = (SHEADER){STYPE_PROCEDURE, sizeof(SPROCEDURE), NULL};\
                topcont->fptr = (void*)rabbit_toplevel;\
                topcont->env = SNULL;\
              saved_call_f = (SVALUE)topcont; \
              saved_call_len = 0; \
              setjmp(entry); \
              perform_saved_call();


#endif