#include "stdlib.h"
#include "string.h"
#include "stdio.h"

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

void check_argument_type(struct SValue v, enum SType t, char* who, long long arg_pos) {
    if (!(v.type == t)){
        printf(who); printf(": "); printf("contract violation "); printf("\n");
        printf("expected: "); printf(unparse_SType(t)); printf("\n");
        printf("given: "); printf(unparse_SType(v.type)); printf("\n");
        printf("at: "); printf("%ll",arg_pos); printf("th argument"); printf("\n");
        exit(-1);
    }
}

void raise_error(char* who, char* message){
    printf(who); printf(": "); printf(message); printf("\n");
    exit(-1);
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

struct SValue make_SBoolean(long long i){
    struct SValue v;
    v.type = BOOLEAN;
    v.ptr = (void*)i;
    return v;
}

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

struct SValue prim_add(struct SValue a, struct SValue b){
    check_argument_type(a, INTEGER, "+" , 1);
    check_argument_type(b, INTEGER, "+" , 2);

    return make_SInteger((long long)a.ptr + (long long)b.ptr);
}

struct SValue prim_add1(struct SValue a){
    check_argument_type(a, INTEGER, "add1", 1);
    return make_SInteger((long long)a.ptr+1);
}

struct SValue prim_sub(struct SValue a, struct SValue b){
    check_argument_type(a, INTEGER, "-", 1);
    check_argument_type(b, INTEGER, "-", 2);

    return  make_SInteger((long long) a.ptr - (long long) b.ptr);
}

struct SValue prim_sub1(struct SValue a){
    check_argument_type(a, INTEGER, "sub1", 1);
    return make_SInteger((long long)a.ptr-1);
}

struct SValue prim_mul(struct SValue a, struct SValue b){
    check_argument_type(a, INTEGER, "*", 1);
    check_argument_type(b, INTEGER, "*", 2);

    return  make_SInteger((long long) a.ptr * (long long) b.ptr);
}

struct SValue prim_div(struct SValue a, struct SValue b){
    check_argument_type(a, INTEGER, "/", 1);
    check_argument_type(b, INTEGER, "/", 2);
    if (b.ptr == 0){
        raise_error("/","division by zero");
    }

    return  make_SInteger((long long) a.ptr / (long long) b.ptr);
}

struct SValue prim_zero(struct SValue a){
    check_argument_type(a, INTEGER, "zero?", 1);
    return (a.ptr == 0)? make_SBoolean(1): make_SBoolean(0);
}

struct SValue prim_displayln(struct SValue a){
    print_SValue(a);
    printf("\n");
    return constant_void;
}

struct SValue prim_make_vector(struct SValue len,struct SValue v){
    check_argument_type(len,INTEGER,"make-vector",1);
    return make_SVector((long long)len.ptr, v);
}

struct SValue prim_vector_set(struct SValue vec, struct SValue i, struct SValue v){
    char who[] = "vector-set!";
    check_argument_type(vec,VECTOR,who,1);
    check_argument_type(i,INTEGER,who,2);
    struct SVector sv= *(struct SVector*)vec.ptr;
    if ((long long)i.ptr >= sv.len){
        raise_error(who, "index outbound");
    }
    struct SValue* arr = sv.vptr;
    arr[(long long)i.ptr] = v;
    return constant_void;
}

struct SValue prim_vector_ref(struct SValue vec, struct SValue i){
    char who[] = "vector-ref";
    check_argument_type(vec,VECTOR,who,1);
    check_argument_type(i,INTEGER,who,2);
    struct SVector sv= *(struct SVector*)vec.ptr;
    if ((long long)i.ptr >= sv.len){
        raise_error(who, "index outbound");
    }
    struct SValue* arr =sv.vptr;
    return arr[(long long)i.ptr];
}

struct SValue prim_make_closure(void* fptr,struct SValue env){
    return make_SProcedure(fptr, env);
}

// current box implementation uses vector
struct SValue prim_box(struct SValue v){
    return make_SVector(1,v);
}

struct SValue prim_set_box(struct SValue b,struct SValue v){
    return prim_vector_set(b,make_SInteger(0),v);
}

struct SValue prim_unbox(struct SValue b){
    return prim_vector_ref(b,make_SInteger(0));
}

void prim_halt(struct SValue v){
    exit(0);
}




struct SValue proc_halt; 

void setup(){
    proc_halt = make_SProcedure(prim_halt, make_SVector(0, constant_void));
}