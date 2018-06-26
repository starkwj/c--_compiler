#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammartree.h"

#define SIZE 10

extern int serror;

typedef struct var {
    char *name;         // 变量名
    int vk;             // 中间代码别名数字，组合成vk 
    Varkind varkind;    // 变量类型
    char *addr;         // 地址
} Var;

typedef struct varlist {
    Varkind varkind;
    struct varlist *next;
} Varlist;

typedef struct fun {
    char *name;         // 函数名
    Varkind retkind;    // 返回值类型
    Varlist *paramlist; // 参数类型列表
    char *addr;         // 地址
} Fun;

typedef struct symbol {
    Kind kind;          // 符号种类
    int level;          // 作用域级别
    union {
        Var *pvar;      // 变量符号
        Fun *pfun;      // 函数符号
    };
    struct symbol *next;
} Symbol;

Symbol *SymbolTable[SIZE];  // Hash符号表

int hash(const char *name);       // hash映射函数

void initSymbolTable();     // 初始化符号表头结点
Varkind getVarkind(const char *type);
int repeatName(Symbol *s, char *name, int level, int line);
void newVarSymbol(char *name, Varkind k, int level, int line);

Varkind getSymbolType(const char *name);
Varkind getExpType(GTNODE *gt);
char *getNameOfVarkind(Varkind v);
void showSymbolTable();
int getMaxLevel();
Varkind findRetType(GTNODE *gt, int *line);
void createSymbolTable(GTNODE *gt, int level, int showTable);  // 创建符号表
void removeSymbolOfLevel(int level);    // 删除指定级的符号




#endif // _SYMBOL_H

