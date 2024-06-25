
#ifndef STYPES
#define STYPES 1
#include "stdio.h"
#include "stdlib.h"

typedef unsigned long long SVALUE;
typedef long long FIXNUM;
#define SVALUE_TYPE(v) ((v) & 0b111)
#define SVALUE_VALUE(v) ((v) >> 3)
#define SVALUE_FIXNUM(v) ((FIXNUM)(v) >> 3)
#define SVALUE_MAKE(t, v) ((SVALUE)((v) << 3) | (SVALUE)(t))
#define CSTRING_MAKE(addr) ((SVALUE)(addr)|(SVALUE)(STYPE_CSTRING))

#define STYPE_POINTER 0
#define STYPE_FIXNUM 1

#define STYPE_CHAR 3
#define STYPE_VOID 4
#define STYPE_BOOLEAN 5
#define STYPE_CSTRING 6

#define STYPE_VECTOR 0
#define STYPE_PROCEDURE 1
#define STYPE_STRING 2
#define STYPE_PAIR 3

typedef struct {
    unsigned long long type;
    size_t len;
    void *forward;
} SHEADER, *pSHEADER;

typedef struct
{
    SHEADER header;
    size_t len;
} SVECTOR, *pSVECTOR;


typedef struct
{
    SHEADER header;
    void *fptr;
    SVALUE env;
} SPROCEDURE, *pSPROCEDURE;

typedef struct
{
    SHEADER header;
    char *sptr;
} SSTRING, *pSSTRING;

typedef struct
{
    SHEADER header;
    SVALUE car;
    SVALUE cdr;
} SPAIR;

extern SVALUE constant_true;
extern SVALUE constant_false;
extern SVALUE constant_void;


#define SVOID constant_void
#define STRUE constant_true
#define SFALSE constant_false
#define SNULL ((SVALUE)0)

SVALUE prim_display(SVALUE v);
#endif