#include "runtime.h"
void scheme_fun0(struct SValue env1,struct SValue x3,struct SValue k2){
struct SValue env2=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _2=prim_vector_set(env2,make_SInteger(0),env1);
struct SValue _1=prim_vector_set(env2,make_SInteger(2),k2);
struct SValue _0=prim_vector_set(env2,make_SInteger(1),x3);
if(!(k2.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc0=*(struct SProcedure*)k2.ptr;
(*(void (*)(struct SValue,struct SValue))proc0.fptr)(proc0.env,x3);
}
void scheme_fun1(struct SValue env5,struct SValue x2,struct SValue k1){
struct SValue env6=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _5=prim_vector_set(env6,make_SInteger(0),env5);
struct SValue _4=prim_vector_set(env6,make_SInteger(2),k1);
struct SValue _3=prim_vector_set(env6,make_SInteger(1),x2);
struct SValue b1=prim_display(make_SString((unsigned char[]){64,0}));
if(!(k1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc1=*(struct SProcedure*)k1.ptr;
(*(void (*)(struct SValue,struct SValue))proc1.fptr)(proc1.env,x2);
}
void scheme_fun2(struct SValue env9,struct SValue x5,struct SValue k4){
struct SValue env10=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _8=prim_vector_set(env10,make_SInteger(0),env9);
struct SValue _7=prim_vector_set(env10,make_SInteger(2),k4);
struct SValue _6=prim_vector_set(env10,make_SInteger(1),x5);
if(!(k4.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc2=*(struct SProcedure*)k4.ptr;
(*(void (*)(struct SValue,struct SValue))proc2.fptr)(proc2.env,x5);
}
void scheme_fun3(struct SValue env13,struct SValue x4,struct SValue k3){
struct SValue env14=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _11=prim_vector_set(env14,make_SInteger(0),env13);
struct SValue _10=prim_vector_set(env14,make_SInteger(2),k3);
struct SValue _9=prim_vector_set(env14,make_SInteger(1),x4);
struct SValue b4=prim_display(make_SString((unsigned char[]){42,0}));
if(!(k3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc3=*(struct SProcedure*)k3.ptr;
(*(void (*)(struct SValue,struct SValue))proc3.fptr)(proc3.env,x4);
}
void scheme_fun4(struct SValue env18,struct SValue x0,struct SValue x1,struct SValue k0){
struct SValue env19=prim_make_vector(make_SInteger(4),make_SBoolean(0));
struct SValue _15=prim_vector_set(env19,make_SInteger(0),env18);
struct SValue _14=prim_vector_set(env19,make_SInteger(3),k0);
struct SValue _13=prim_vector_set(env19,make_SInteger(2),x1);
struct SValue _12=prim_vector_set(env19,make_SInteger(1),x0);
if(!(x0.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc4=*(struct SProcedure*)x0.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc4.fptr)(proc4.env,x1,k0);
}
void scheme_fun5(struct SValue env15,struct SValue b3){
struct SValue env16=prim_make_vector(make_SInteger(2),make_SBoolean(0));
struct SValue _17=prim_vector_set(env16,make_SInteger(0),env15);
struct SValue _16=prim_vector_set(env16,make_SInteger(1),b3);
struct SValue env17=prim_vector_ref(env15,make_SInteger(0));
struct SValue b0=prim_vector_ref(env17,make_SInteger(1));
struct SValue clo4=prim_make_closure(scheme_fun4,env16);
if(!(clo4.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc5=*(struct SProcedure*)clo4.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue,struct SValue))proc5.fptr)(proc5.env,b0,b3,proc_halt);
}
void scheme_fun6(struct SValue env11,struct SValue b5){
struct SValue env12=prim_make_vector(make_SInteger(2),make_SBoolean(0));
struct SValue _19=prim_vector_set(env12,make_SInteger(0),env11);
struct SValue _18=prim_vector_set(env12,make_SInteger(1),b5);
struct SValue clo3=prim_make_closure(scheme_fun3,env12);
struct SValue clo5=prim_make_closure(scheme_fun5,env12);
if(!(clo3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc6=*(struct SProcedure*)clo3.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc6.fptr)(proc6.env,b5,clo5);
}
void scheme_fun7(struct SValue env7,struct SValue b0){
struct SValue env8=prim_make_vector(make_SInteger(2),make_SBoolean(0));
struct SValue _21=prim_vector_set(env8,make_SInteger(0),env7);
struct SValue _20=prim_vector_set(env8,make_SInteger(1),b0);
struct SValue clo2=prim_make_closure(scheme_fun2,env8);
struct SValue clo6=prim_make_closure(scheme_fun6,env8);
if(!(proc_callcc.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc7=*(struct SProcedure*)proc_callcc.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc7.fptr)(proc7.env,clo2,clo6);
}
void scheme_fun8(struct SValue env3,struct SValue b2){
struct SValue env4=prim_make_vector(make_SInteger(2),make_SBoolean(0));
struct SValue _23=prim_vector_set(env4,make_SInteger(0),env3);
struct SValue _22=prim_vector_set(env4,make_SInteger(1),b2);
struct SValue clo1=prim_make_closure(scheme_fun1,env4);
struct SValue clo7=prim_make_closure(scheme_fun7,env4);
if(!(clo1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc8=*(struct SProcedure*)clo1.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc8.fptr)(proc8.env,b2,clo7);
}
int main(){
setup();
struct SValue env0=prim_make_vector(make_SInteger(0),make_SBoolean(0));
struct SValue clo0=prim_make_closure(scheme_fun0,env0);
struct SValue clo8=prim_make_closure(scheme_fun8,env0);
if(!(proc_callcc.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc9=*(struct SProcedure*)proc_callcc.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc9.fptr)(proc9.env,clo0,clo8);
}
