
enum SType {
    VOID,
    INTEGER,
    BOOLEAN, // UNBOXED TYPES
    STRING,  // C String
    VECTOR, // 
    PROCEDURE
};
struct SValue {
    enum SType type;
    void* ptr;
};
struct SVector {
    long long len;
    struct SValue* vptr;
};
struct SProcedure {
    void* fptr;
    struct SValue env;
};

char* unparse_SType(enum SType type){
    if (type == VOID){
        return "void";
    }
    if (type == INTEGER) {
        return "integer";
    }
    if (type == BOOLEAN){
        return "boolean";
    }
    if (type == STRING){
        return "string";
    }
    if (type == VECTOR){
        return "vector";
    }
    if (type == PROCEDURE){
        return "procedure";
    }
    return "null"; //impossible
}


void print_SValue(struct SValue v);

struct SValue constant_void = {VOID,0};
struct SValue make_SInteger(long long i){
    struct SValue v;
    v.type = INTEGER;
    v.ptr = (void*)i;
    return v;
}

void print_SInteger(struct SValue v){
    printf("%lli", (long long) v.ptr);
}

struct SValue constant_true = {BOOLEAN, (void*)0};
struct SValue constant_false = {BOOLEAN, (void*)1};

void print_SBoolean(struct SValue v){
    if(v.ptr){
        printf("#t");
    } else {
        printf("#f");
    }
}

struct SValue make_SString(char* str){
    struct SValue v;
    v.type = STRING;
    v.ptr = (void*) str;
    return v;
}

void print_SString(struct SValue v){
    printf("%s",(char*) v.ptr);
}

struct SValue make_SVector(long long len,struct SValue init){
    struct SVector* sv = malloc(sizeof(struct SVector));
    sv->len = len;
    sv->vptr = malloc(len * sizeof(struct SValue));
    
    // fill the vector with init
    struct SValue* arr=sv->vptr;
    for(long long i = 0; i< len; i++){
        arr[i] = init;
    }
    
    struct SValue v;
    v.type = VECTOR;
    v.ptr = sv;
    
    return v;
}

void print_SVector(struct SValue v){
    struct SVector* svp = v.ptr;
    struct SVector sv = *svp;
    long long len = sv.len;

    struct SValue* arr = sv.vptr; // SValue Array
    printf("#(");
    for (long long i = 0; i<len; i++){
        struct SValue v = arr[i];
        print_SValue(v);
        if (i != len -1) printf(",");
    }
    printf(")");
    
}

struct SValue make_SProcedure(void* fptr,struct SValue env){
    struct SProcedure* proc = malloc(sizeof(struct SProcedure));
    proc->fptr = fptr;
    proc->env = env;

    struct SValue v;
    v.type = PROCEDURE;
    v.ptr = proc;
    return  v;
}

void print_SProcedure(struct SValue v){
    printf("#<procedure>");
}

void print_SValue(struct SValue v){
    enum SType type = v.type;
    if (type == VOID){
        printf("#<void>");
        return;
    }
    if (type == INTEGER) {
        print_SInteger(v);
        return;
    }
    if (type == BOOLEAN){
        print_SBoolean(v);
        return;
    }
    if (type == STRING){
        print_SString(v);
        return;
    }
    if (type == VECTOR){
        print_SVector(v);
        return;
    }
    if (type == PROCEDURE){
        print_SProcedure(v);
        return;
    }
}

