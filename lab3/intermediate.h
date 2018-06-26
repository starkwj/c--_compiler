#ifndef _INTERMEDIATE
#define _INTERMEDIATE

#include <stdio.h>
#include "symbol.h"



typedef struct code {
    char sentense[28];
    struct code *next;
} Code;

typedef struct arg {
    char a[4];
    struct arg *next;
} Arg;

extern int curtemp;
extern int curvar;
extern int curlabel;

int newTemp();
char *newTempPlace();
int newVar();
Code *newCode();

// 给定变量名，返回别名
char *lookup(const char *id);

char *getConstOrId(GTNODE *gt);
char *newLabel();


Code *translate_Exp(GTNODE *gt, char *place);
Code *translate_Args(GTNODE *gt, Arg *arglist);
Code *translate_Stmt(GTNODE *gt);
Code *translate_Cond(GTNODE *gt, char *label_true, char *label_false);
Code *translate_CompSt(GTNODE *gt);
Code *translate_Fundec(GTNODE *gt);
Code *translate_Dec(GTNODE *gt);

void createInterCode(GTNODE *gt, int level, int trstmt, Code *ctail);
Code *getCodeTail(Code *c);
void printCode(Code *c, int tofile);

#endif // _INTERMEDIATE
