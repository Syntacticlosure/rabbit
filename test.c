#include "runtime.h"
int main(){
setup();
struct SValue env0=prim_make_vector(make_SInteger(0),make_SBoolean(0));
struct SValue b0=prim_displayln(make_SString((unsigned char[]){104,101,108,108,111,32,119,111,114,108,100,0}));
if(!(proc_halt.type == PROCEDURE)) { raise_error("application","not a procedure"); }
struct SProcedure proc0=*(struct SProcedure*)proc_halt.ptr;
(*(void (*)(struct SValue,struct SValue))proc0.fptr)(proc0.env,b0);
}