#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stypes.h"

void check_argument_type(struct SValue v, enum SType t, char* who, long long arg_pos) {
    if (!(v.type == t)){
        printf("%s: contract violation\n", who);
        printf("expected: %s\n", unparse_SType(t));
        printf("given: %s\n", unparse_SType(v.type)); 
        printf("at: %lli th argument\n", arg_pos);
        exit(-1);
    }
}

void raise_error(char* who, char* message){
    printf("%s: %s\n",who,message);
    exit(-1);
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
    return (a.ptr == 0)? constant_true: constant_false;
}

struct SValue prim_displayln(struct SValue a){
    print_SValue(a);
    printf("\n");
    return constant_void;
}

struct SValue prim_display(struct SValue a){
    print_SValue(a);
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


void fun_halt(struct SValue v){
    exit(0);
}

void fun_contwrapper(struct SValue env, struct SValue x, struct SValue k_ignore){
    struct SValue k = prim_vector_ref(env,make_SInteger(0));
    // CONTINUATION MUST BE A PROCEDURE
    struct SProcedure proc =*(struct SProcedure*)k.ptr;
    (*(void (*)(struct SValue,struct SValue))proc.fptr)(proc.env,x);
}

void fun_callcc(struct SValue env,struct SValue f,struct SValue k){
    if (f.type!= PROCEDURE){
        raise_error("application: ", "not a procedure");
    }
    struct SProcedure proc = *(struct SProcedure*)f.ptr;
    // wrap k 
    struct SValue cont = make_SProcedure(fun_contwrapper, prim_box(k));
    (*(void (*)(struct SValue,struct SValue,struct SValue))proc.fptr)(proc.env,cont,k);
}

struct SValue proc_halt; 
struct SValue proc_callcc;
// setup global environment
void setup(){
    struct SValue empty_env = make_SVector(0, constant_void);
    proc_halt = make_SProcedure(fun_halt, empty_env);
    proc_callcc = make_SProcedure(fun_callcc,empty_env);
}