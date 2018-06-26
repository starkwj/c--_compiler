#ifndef _OBJECT_H
#define _OBJECT_H

#include "intermediate.h"
#define REGNUM 8


extern int *VA;
extern int *TA;

void initVsym(int v, int t);

void tran_assign(Code *code, FILE *f);
void tran_plus(Code *code ,FILE *f);
void tran_minus(Code *code ,FILE *f);
void tran_multi(Code *code ,FILE *f);
void tran_div(Code *code ,FILE *f);
void tran_arg(Code *code, FILE *f);
void tran_callassign(Code *code, FILE *f);
void tran_return(Code *code, FILE *f);
void tran_param(Code *code, FILE *f);
void tran_if(Code *code, FILE *f);
void tran_write(Code *code, FILE *f);
void initCode(FILE *f);

void printObjectCode(Code *code);


#endif // _OBJECT_H
