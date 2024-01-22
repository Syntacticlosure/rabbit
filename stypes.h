
enum SType
{
    VOID,
    INTEGER,
    BOOLEAN, // UNBOXED TYPES
    STRING,  // C String
    VECTOR,  //
    PROCEDURE
};
struct SValue
{
    enum SType type;
    void *ptr;
};

struct SVector
{
    long long len;
    struct SValue *vptr;
};
struct SProcedure
{
    void *fptr;
    struct SValue env;
};

struct SValue constant_true = {BOOLEAN, (void *)1};
struct SValue constant_false = {BOOLEAN, (void *)0};
struct SValue constant_void = {VOID, 0};

#define S_VALUE(type, ptr) ((struct SValue){(type), (ptr)})
#define S_INTEGER(i) S_VALUE(INTEGER, (void *)(i))
#define S_STRING(s) S_VALUE(STRING, (void *)(s))
#define S_UNINIT_VECTOR(len) S_VALUE(VECTOR, ((void *)&(struct SVector){((long long) ((len).ptr)), alloca(((long long) ((len).ptr)) * sizeof(struct SValue))}))
#define S_PROCEDURE(f, env) S_VALUE(PROCEDURE, ((void *)&(struct SProcedure){(f), (env)}))

char *unparse_SType(enum SType type)
{
    return type == VOID        ? "void"
           : type == INTEGER   ? "integer"
           : type == BOOLEAN   ? "boolean"
           : type == STRING    ? "string"
           : type == VECTOR    ? "vector"
           : type == PROCEDURE ? "procedure"
           : type == VECTOR    ? "vector"
                               : "impossible";
}

void print_SValue(struct SValue v);

void print_SVoid(struct SValue v)
{
    printf("#<void>");
}

void print_SInteger(struct SValue v)
{
    printf("%lli", (long long)v.ptr);
}

void print_SBoolean(struct SValue v)
{
    printf("%s", v.ptr ? "#t" : "#f");
}

void print_SString(struct SValue v)
{
    printf("%s", (char *)v.ptr);
}

void print_SVector(struct SValue v)
{
    struct SVector sv = *(struct SVector*)v.ptr;
    long long len = sv.len;

    struct SValue *arr = sv.vptr; // SValue Array
    printf("#(");
    for (long long i = 0; i < len; i++)
    {
        struct SValue v = arr[i];
        print_SValue(v);
        if (i != len - 1)
            printf(",");
    }
    printf(")");
}

void print_SProcedure(struct SValue v)
{
    printf("#<procedure>");
}

void print_SValue(struct SValue v)
{
    enum SType type = v.type;
    switch(type){
        case VOID: print_SVoid(v); break;
        case INTEGER: print_SInteger(v); break;
        case BOOLEAN: print_SBoolean(v); break;
        case STRING: print_SString(v); break;
        case VECTOR: print_SVector(v); break;
        case PROCEDURE: print_SProcedure(v); break;
        // impossible
    }
}

void init_SVector(struct SValue v, struct SValue init)
{
    struct SVector sv = *(struct SVector*)v.ptr;

    // fill the vector with init
    struct SValue *arr = sv.vptr;
    for (long long i = 0; i < sv.len; i++)
    {
        arr[i] = init;
    }
}
// primitives for vector operations
struct SValue SVector_ref(struct SValue vec, struct SValue i){
    struct SVector sv= *(struct SVector*)vec.ptr;
    struct SValue* arr =sv.vptr;
    return arr[(long long)i.ptr];
}

struct SValue SVector_len(struct SValue vec){
    struct SVector sv= *(struct SVector*)vec.ptr;
    return S_INTEGER(sv.len);
}

struct SValue SVector_set(struct SValue vec, struct SValue i, struct SValue v){
    struct SVector sv= *(struct SVector*)vec.ptr;
    struct SValue* arr = sv.vptr;
    arr[(long long)i.ptr] = v;
    return constant_void;
}


