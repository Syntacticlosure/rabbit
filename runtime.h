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


void fun_halt(struct SValue env,struct SValue v){
    exit(0);
}

void fun_contwrapper(struct SValue env, struct SValue x, struct SValue k_ignore){
    struct SValue k = SVector_ref(env,S_INTEGER(0));
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
    struct SValue env_ = S_UNINIT_VECTOR(S_INTEGER(1));
    init_SVector(env_,k);
    struct SValue cont = S_PROCEDURE(fun_contwrapper, env_);
    (*(void (*)(struct SValue,struct SValue,struct SValue))proc.fptr)(proc.env,cont,k);
}

struct SValue empty_env;
struct SValue proc_halt;
struct SValue proc_callcc;
// setup global environment

#define SETUP empty_env = S_UNINIT_VECTOR(S_INTEGER(0)); proc_halt =S_PROCEDURE(fun_halt, empty_env); proc_callcc = S_PROCEDURE(fun_callcc,empty_env); 