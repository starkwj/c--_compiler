/**
 * Author: starkwj
 * Created on: 2018-05-29
 */
#ifndef _GRAMMARTREE_h
#define _GRAMMARTREE_h

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

extern int yylineno;    // 行号
extern char* yytext;    // 词
extern FILE* yyin;      // 文件
extern int wordparse;   // 词法分析
extern int yyparse();

// 语法树采用孩子兄弟表示法
typedef struct GrammarTree {
    int line;                   // 行号
    char *name;                 // 语法单元名字
    struct GrammarTree *lchild; // 第一个孩子
    struct GrammarTree *rchild; // 兄弟
    union {
        char *strval;
        char charval;
        int intval;
        float floatval;
    };
} GTNODE;

/* 构造语法树，name: 语法单元名字，num：变长参数中语法结点个数 */
GTNODE * createTree(char *name, int num, ...);

/* 遍历抽象语法树，level为数的层数 */
void traverseTree(GTNODE *gt, int level);

/* 错误处理函数 */
void yyerror(const char *s, ...);

#endif // _GRAMMARTREE_h
