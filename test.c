#include "runtime.h"
void scheme_fun0(struct SValue env5,struct SValue x2,struct SValue k2){
struct SValue env6=prim_make_vector(make_SInteger(3),constant_false);
struct SValue _2=prim_vector_set(env6,make_SInteger(0),env5);
struct SValue _1=prim_vector_set(env6,make_SInteger(2),k2);
struct SValue _0=prim_vector_set(env6,make_SInteger(1),x2);
struct SValue x1=prim_vector_ref(env5,make_SInteger(2));
struct SValue b1=prim_zero(x2);
if(b1.ptr) {
if(!(k2.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc0=*(struct SProcedure*)k2.ptr;
(*(void (*)(struct SValue,struct SValue))proc0.fptr)(proc0.env,constant_true);
}else{
struct SValue b2=prim_unbox(x1);
struct SValue b3=prim_sub1(x2);
if(!(b2.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc1=*(struct SProcedure*)b2.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc1.fptr)(proc1.env,b3,k2);
}
}
void scheme_fun1(struct SValue env7,struct SValue x3,struct SValue k3){
struct SValue env8=prim_make_vector(make_SInteger(3),constant_false);
struct SValue _5=prim_vector_set(env8,make_SInteger(0),env7);
struct SValue _4=prim_vector_set(env8,make_SInteger(2),k3);
struct SValue _3=prim_vector_set(env8,make_SInteger(1),x3);
struct SValue x0=prim_vector_ref(env7,make_SInteger(1));
struct SValue b5=prim_zero(x3);
if(b5.ptr) {
if(!(k3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc2=*(struct SProcedure*)k3.ptr;
(*(void (*)(struct SValue,struct SValue))proc2.fptr)(proc2.env,constant_false);
}else{
struct SValue b6=prim_unbox(x0);
struct SValue b7=prim_sub1(x3);
if(!(b6.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc3=*(struct SProcedure*)b6.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc3.fptr)(proc3.env,b7,k3);
}
}
void scheme_fun2(struct SValue env9,struct SValue b9){
struct SValue env10=prim_make_vector(make_SInteger(2),constant_false);
struct SValue _7=prim_vector_set(env10,make_SInteger(0),env9);
struct SValue _6=prim_vector_set(env10,make_SInteger(1),b9);
struct SValue k1=prim_vector_ref(env9,make_SInteger(3));
struct SValue b8=prim_displayln(b9);
if(!(k1.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc4=*(struct SProcedure*)k1.ptr;
(*(void (*)(struct SValue,struct SValue))proc4.fptr)(proc4.env,b8);
}
void scheme_fun3(struct SValue env3,struct SValue x0,struct SValue x1,struct SValue k1){
struct SValue env4=prim_make_vector(make_SInteger(4),constant_false);
struct SValue _11=prim_vector_set(env4,make_SInteger(0),env3);
struct SValue _10=prim_vector_set(env4,make_SInteger(3),k1);
struct SValue _9=prim_vector_set(env4,make_SInteger(2),x1);
struct SValue _8=prim_vector_set(env4,make_SInteger(1),x0);
struct SValue clo0=prim_make_closure(scheme_fun0,env4);
struct SValue clo1=prim_make_closure(scheme_fun1,env4);
struct SValue clo2=prim_make_closure(scheme_fun2,env4);
struct SValue b0=prim_set_box(x0,clo0);
struct SValue b4=prim_set_box(x1,clo1);
struct SValue b10=prim_unbox(x0);
if(!(b10.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc5=*(struct SProcedure*)b10.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue))proc5.fptr)(proc5.env,make_SInteger(9),clo2);
}
void scheme_fun4(struct SValue env1,struct SValue x0,struct SValue x1,struct SValue k0){
struct SValue env2=prim_make_vector(make_SInteger(4),constant_false);
struct SValue _15=prim_vector_set(env2,make_SInteger(0),env1);
struct SValue _14=prim_vector_set(env2,make_SInteger(3),k0);
struct SValue _13=prim_vector_set(env2,make_SInteger(2),x1);
struct SValue _12=prim_vector_set(env2,make_SInteger(1),x0);
struct SValue clo3=prim_make_closure(scheme_fun3,env2);
struct SValue b11=prim_box(x0);
struct SValue b12=prim_box(x1);
if(!(clo3.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc6=*(struct SProcedure*)clo3.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue,struct SValue))proc6.fptr)(proc6.env,b11,b12,k0);
}
int main(){
setup();
struct SValue env0=prim_make_vector(make_SInteger(0),constant_false);
struct SValue clo4=prim_make_closure(scheme_fun4,env0);
if(!(clo4.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc7=*(struct SProcedure*)clo4.ptr;
(*(void (*)(struct SValue,struct SValue,struct SValue,struct SValue))proc7.fptr)(proc7.env,constant_false,constant_false,proc_halt);
}