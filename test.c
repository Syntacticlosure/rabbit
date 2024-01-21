#include "runtime.h"
void scheme_fun0(struct SValue env12,struct SValue b2){
struct SValue env13=prim_make_vector(make_SInteger(2),make_SBoolean(0));
struct SValue _1=prim_vector_set(env13,make_SInteger(0),env12);
struct SValue _0=prim_vector_set(env13,make_SInteger(1),b2);
struct SValue env14=prim_vector_ref(env12,make_SInteger(0));
struct SValue env15=prim_vector_ref(env14,make_SInteger(0));
struct SValue env16=prim_vector_ref(env15,make_SInteger(0));
struct SValue k2=prim_vector_ref(env15,make_SInteger(2));
struct SValue x0=prim_vector_ref(env16,make_SInteger(1));
struct SValue b4=prim_unbox(x0);
struct SValue b3=prim_displayln(b4);
if(!(k2.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc0=*(struct SProcedure*)k2.ptr;
(*(void (*)(struct SValue,struct SValue))proc0.fptr)(proc0.env,b3);
}
void scheme_fun1(struct SValue env9,struct SValue b1){
struct SValue env10=prim_make_vector(make_SInteger(2),make_SBoolean(0));
struct SValue _3=prim_vector_set(env10,make_SInteger(0),env9);
struct SValue _2=prim_vector_set(env10,make_SInteger(1),b1);
struct SValue env11=prim_vector_ref(env9,make_SInteger(0));
struct SValue x1=prim_vector_ref(env11,make_SInteger(1));
struct SValue clo0=prim_make_closure(scheme_fun0,env10);
if(!(x1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc1=*(struct SProcedure*)x1.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc1.fptr)(proc1.env,make_SInteger(9),clo0);
}
void scheme_fun2(struct SValue env7,struct SValue b0){
struct SValue env8=prim_make_vector(make_SInteger(2),make_SBoolean(0));
struct SValue _5=prim_vector_set(env8,make_SInteger(0),env7);
struct SValue _4=prim_vector_set(env8,make_SInteger(1),b0);
struct SValue x1=prim_vector_ref(env7,make_SInteger(1));
struct SValue clo1=prim_make_closure(scheme_fun1,env8);
if(!(x1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc2=*(struct SProcedure*)x1.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc2.fptr)(proc2.env,make_SInteger(100),clo1);
}
void scheme_fun3(struct SValue env5,struct SValue x1,struct SValue k2){
struct SValue env6=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _8=prim_vector_set(env6,make_SInteger(0),env5);
struct SValue _7=prim_vector_set(env6,make_SInteger(2),k2);
struct SValue _6=prim_vector_set(env6,make_SInteger(1),x1);
struct SValue clo2=prim_make_closure(scheme_fun2,env6);
if(!(x1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc3=*(struct SProcedure*)x1.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc3.fptr)(proc3.env,make_SInteger(10),clo2);
}
void scheme_fun4(struct SValue env17,struct SValue x2,struct SValue k3){
struct SValue env18=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _11=prim_vector_set(env18,make_SInteger(0),env17);
struct SValue _10=prim_vector_set(env18,make_SInteger(2),k3);
struct SValue _9=prim_vector_set(env18,make_SInteger(1),x2);
struct SValue x0=prim_vector_ref(env17,make_SInteger(1));
struct SValue b7=prim_unbox(x0);
struct SValue b6=prim_add(x2,b7);
struct SValue b5=prim_set_box(x0,b6);
if(!(k3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc4=*(struct SProcedure*)k3.ptr;
(*(void (*)(struct SValue,struct SValue))proc4.fptr)(proc4.env,b5);
}
void scheme_fun5(struct SValue env3,struct SValue x0,struct SValue k1){
struct SValue env4=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _14=prim_vector_set(env4,make_SInteger(0),env3);
struct SValue _13=prim_vector_set(env4,make_SInteger(2),k1);
struct SValue _12=prim_vector_set(env4,make_SInteger(1),x0);
struct SValue clo3=prim_make_closure(scheme_fun3,env4);
struct SValue clo4=prim_make_closure(scheme_fun4,env4);
if(!(clo3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc5=*(struct SProcedure*)clo3.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc5.fptr)(proc5.env,clo4,k1);
}
void scheme_fun6(struct SValue env1,struct SValue x0,struct SValue k0){
struct SValue env2=prim_make_vector(make_SInteger(3),make_SBoolean(0));
struct SValue _17=prim_vector_set(env2,make_SInteger(0),env1);
struct SValue _16=prim_vector_set(env2,make_SInteger(2),k0);
struct SValue _15=prim_vector_set(env2,make_SInteger(1),x0);
struct SValue clo5=prim_make_closure(scheme_fun5,env2);
struct SValue b8=prim_box(x0);
if(!(clo5.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc6=*(struct SProcedure*)clo5.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc6.fptr)(proc6.env,b8,k0);
}
int main(){
setup();
struct SValue env0=prim_make_vector(make_SInteger(0),make_SBoolean(0));
struct SValue clo6=prim_make_closure(scheme_fun6,env0);
if(!(clo6.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc7=*(struct SProcedure*)clo6.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc7.fptr)(proc7.env,make_SInteger(0),proc_halt);
}