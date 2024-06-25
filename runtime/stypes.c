#include "stypes.h"
SVALUE constant_true = SVALUE_MAKE(STYPE_BOOLEAN, 1);
SVALUE constant_false = SVALUE_MAKE(STYPE_BOOLEAN, 0);
SVALUE constant_void = SVALUE_MAKE(STYPE_VOID, 0);

SVALUE prim_display(SVALUE v)
{
    if (v == 0)
    {
        printf("#<null>");
        return SVOID;
    }
    SVALUE type = SVALUE_TYPE(v);
    if (type == STYPE_POINTER)
    {
        pSHEADER header = (pSHEADER)v;
        if (header->type == STYPE_VECTOR)
        {
            pSVECTOR vec = (pSVECTOR)v;
            SVALUE *start = (SVALUE *)((void *)v + sizeof(SVECTOR));
            printf("#(");
            for (size_t i = 0; i < vec->len; i++)
            {
                prim_display(start[i]);
                if (i == vec->len - 1)
                {
                    printf(",");
                }
            }
            printf(")");
        }
        else if (header->type == STYPE_PROCEDURE)
        {
            printf("#<PROCEDURE>");
        }
        else if (header->type == STYPE_STRING)
        {
            char *start = (char *)((void *)v + sizeof(SSTRING));
            printf("%s", start);
        }
        else if (header->type == STYPE_PAIR)
        {
            printf("(");
            SPAIR* pair = (SPAIR*)v;
            prim_display(pair->car);
            printf(",");
            prim_display(pair->cdr);
            printf(")");
        }
    }
    else if (type == STYPE_FIXNUM)
    {
        printf("%lli", SVALUE_FIXNUM(v));
    }
    else if (type == STYPE_CHAR)
    {
        printf("%c", (char)(SVALUE_VALUE(v)));
    }
    else if (type == STYPE_VOID)
    {
        printf("#<void>");
    }
    else if (type == STYPE_BOOLEAN)
    {
        printf(v == constant_true ? "#t" : "#f");
    }
    else if (type == STYPE_CSTRING)
    {
        printf("%s", (v & (~(SVALUE)0b111)));
    }

    return SVOID;
}