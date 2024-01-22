#include "runtime.h"
void scheme_fun0(struct SValue env1,struct SValue x3,struct SValue k2){
struct SValue env2 = S_UNINIT_VECTOR(S_INTEGER(3));
init_SVector(env2,constant_false);
struct SValue _2 = SVector_set(env2,S_INTEGER(0),env1);
struct SValue _1 = SVector_set(env2,S_INTEGER(2),k2);
struct SValue _0 = SVector_set(env2,S_INTEGER(1),x3);
if(!(k2.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc0=*(struct SProcedure*)k2.ptr;
(*(void (*)(struct SValue,struct SValue))proc0.fptr)(proc0.env,x3);
}
void scheme_fun1(struct SValue env5,struct SValue x2,struct SValue k1){
struct SValue env6 = S_UNINIT_VECTOR(S_INTEGER(3));
init_SVector(env6,constant_false);
struct SValue _5 = SVector_set(env6,S_INTEGER(0),env5);
struct SValue _4 = SVector_set(env6,S_INTEGER(2),k1);
struct SValue _3 = SVector_set(env6,S_INTEGER(1),x2);
print_SValue(S_STRING(((unsigned char[]){64,0})));
struct SValue b1 = constant_void;
if(!(k1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc1=*(struct SProcedure*)k1.ptr;
(*(void (*)(struct SValue,struct SValue))proc1.fptr)(proc1.env,x2);
}
void scheme_fun2(struct SValue env9,struct SValue x5,struct SValue k4){
struct SValue env10 = S_UNINIT_VECTOR(S_INTEGER(3));
init_SVector(env10,constant_false);
struct SValue _8 = SVector_set(env10,S_INTEGER(0),env9);
struct SValue _7 = SVector_set(env10,S_INTEGER(2),k4);
struct SValue _6 = SVector_set(env10,S_INTEGER(1),x5);
if(!(k4.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc2=*(struct SProcedure*)k4.ptr;
(*(void (*)(struct SValue,struct SValue))proc2.fptr)(proc2.env,x5);
}
void scheme_fun3(struct SValue env13,struct SValue x4,struct SValue k3){
struct SValue env14 = S_UNINIT_VECTOR(S_INTEGER(3));
init_SVector(env14,constant_false);
struct SValue _11 = SVector_set(env14,S_INTEGER(0),env13);
struct SValue _10 = SVector_set(env14,S_INTEGER(2),k3);
struct SValue _9 = SVector_set(env14,S_INTEGER(1),x4);
print_SValue(S_STRING(((unsigned char[]){42,0})));
struct SValue b4 = constant_void;
if(!(k3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc3=*(struct SProcedure*)k3.ptr;
(*(void (*)(struct SValue,struct SValue))proc3.fptr)(proc3.env,x4);
}
void scheme_fun4(struct SValue env18,struct SValue x0,struct SValue x1,struct SValue k0){
struct SValue env19 = S_UNINIT_VECTOR(S_INTEGER(4));
init_SVector(env19,constant_false);
struct SValue _15 = SVector_set(env19,S_INTEGER(0),env18);
struct SValue _14 = SVector_set(env19,S_INTEGER(3),k0);
struct SValue _13 = SVector_set(env19,S_INTEGER(2),x1);
struct SValue _12 = SVector_set(env19,S_INTEGER(1),x0);
if(!(x0.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc4=*(struct SProcedure*)x0.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc4.fptr)(proc4.env,x1,k0);
}
void scheme_fun5(struct SValue env15,struct SValue b3){
struct SValue env16 = S_UNINIT_VECTOR(S_INTEGER(2));
init_SVector(env16,constant_false);
struct SValue _17 = SVector_set(env16,S_INTEGER(0),env15);
struct SValue _16 = SVector_set(env16,S_INTEGER(1),b3);
struct SValue env17 = SVector_ref(env15,S_INTEGER(0));
struct SValue b0 = SVector_ref(env17,S_INTEGER(1));
struct SValue clo4 = S_PROCEDURE(scheme_fun4,env16);
if(!(clo4.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc5=*(struct SProcedure*)clo4.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue,struct SValue))proc5.fptr)(proc5.env,b0,b3,proc_halt);
}
void scheme_fun6(struct SValue env11,struct SValue b5){
struct SValue env12 = S_UNINIT_VECTOR(S_INTEGER(2));
init_SVector(env12,constant_false);
struct SValue _19 = SVector_set(env12,S_INTEGER(0),env11);
struct SValue _18 = SVector_set(env12,S_INTEGER(1),b5);
struct SValue clo3 = S_PROCEDURE(scheme_fun3,env12);
struct SValue clo5 = S_PROCEDURE(scheme_fun5,env12);
if(!(clo3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc6=*(struct SProcedure*)clo3.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc6.fptr)(proc6.env,b5,clo5);
}
void scheme_fun7(struct SValue env7,struct SValue b0){
struct SValue env8 = S_UNINIT_VECTOR(S_INTEGER(2));
init_SVector(env8,constant_false);
struct SValue _21 = SVector_set(env8,S_INTEGER(0),env7);
struct SValue _20 = SVector_set(env8,S_INTEGER(1),b0);
struct SValue clo2 = S_PROCEDURE(scheme_fun2,env8);
struct SValue clo6 = S_PROCEDURE(scheme_fun6,env8);
if(!(proc_callcc.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc7=*(struct SProcedure*)proc_callcc.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc7.fptr)(proc7.env,clo2,clo6);
}
void scheme_fun8(struct SValue env3,struct SValue b2){
struct SValue env4 = S_UNINIT_VECTOR(S_INTEGER(2));
init_SVector(env4,constant_false);
struct SValue _23 = SVector_set(env4,S_INTEGER(0),env3);
struct SValue _22 = SVector_set(env4,S_INTEGER(1),b2);
struct SValue clo1 = S_PROCEDURE(scheme_fun1,env4);
struct SValue clo7 = S_PROCEDURE(scheme_fun7,env4);
if(!(clo1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc8=*(struct SProcedure*)clo1.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc8.fptr)(proc8.env,b2,clo7);
}
int main(){
SETUP
struct SValue env0 = S_UNINIT_VECTOR(S_INTEGER(0));
init_SVector(env0,constant_false);
struct SValue clo0 = S_PROCEDURE(scheme_fun0,env0);
struct SValue clo8 = S_PROCEDURE(scheme_fun8,env0);
if(!(proc_callcc.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc9=*(struct SProcedure*)proc_callcc.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc9.fptr)(proc9.env,clo0,clo8);
}

