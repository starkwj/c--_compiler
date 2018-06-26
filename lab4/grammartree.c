/**
 * Author: starkwj
 * Created on: 2018-05-29
 */

#include "grammartree.h"

GTNODE * createTree(char *name, int num, ...)
{
    va_list valist; // 变长参数列表
    va_start(valist, num);  // 初始化变长参数为num后的参数

    GTNODE *gt = (GTNODE *)malloc(sizeof(GTNODE));
    GTNODE *tmp;
    gt->name = name;
    gt->lchild = gt->rchild = NULL;
    gt->type = VOID;
    gt->place = NULL;

    if (num > 0)    // 非终结符，变长参数为语法数结点
    {
        // 将第一个结点赋给gt的左孩子
        tmp = va_arg(valist, GTNODE *);
        gt->lchild = tmp;
        gt->line = tmp->line;
        // 剩余节点设置为兄弟结点
        int i;
        for (i = 0; i < num - 1; i++)
        {
            tmp->rchild = va_arg(valist, GTNODE *);
            tmp = tmp->rchild;
        }
    }
    else    // 终结符或产生空的语法单元
    {
        gt->line = va_arg(valist, int);
        if (!strcmp(gt->name, "IDENTIFIER") || !strcmp(gt->name, "TYPE") || !strcmp(gt->name, "RELOP"))
        {
            char *str = (char *)malloc(sizeof(char) * (strlen(yytext) + 1));
            strcpy(str, yytext);
            gt->strval = str;
        }
        else if (!strcmp(gt->name, "CONSTBOOL"))
        {
            // char *str = (char *)malloc(sizeof(char) * (strlen(yytext) + 1));
            // strcpy(str, yytext);
            // gt->strval = str;
            if (!strcmp(yytext, "true")) {
                gt->intval = 1;
            } else {
                gt->intval = 0;
            }
            gt->type = BOOL;
        }
        else if (!strcmp(gt->name, "CONSTSTRING"))
        {
            char *str = (char *)malloc(sizeof(char) * (strlen(yytext) + 1));
            strcpy(str, yytext);
            gt->strval = str;
            gt->type = STRING;
        }
        else if (!strcmp(gt->name, "CONSTINTD"))
        {
            gt->intval = atoi(yytext);
            gt->type = INT;
        }
        else if (!strcmp(gt->name, "CONSTINTH"))
        {
            gt->intval = strtol(yytext, NULL, 16);
            gt->type = INT;
        }
        else if (!strcmp(gt->name, "CONSTFLOAT"))
        {
            gt->floatval = atof(yytext);
            gt->type = FLOAT;
        }
        else if (!strcmp(gt->name, "CONSTCHAR"))
        {
            gt->charval = yytext[1];
            gt->type = CHAR;
        }

    }
    return gt;
}

void traverseTree(GTNODE *gt, int level)
{
    if (gt != NULL)
    {
        // 控制缩进
        int i;
        for (i = 0; i < level; i++)
            printf("  ");

        if (gt->line != -1)
        {
            printf("%s: ", gt->name); // 打印语法单元名字
            if (!strcmp(gt->name, "IDENTIFIER") || !strcmp(gt->name, "CONSTBOOL")
                || !strcmp(gt->name, "CONSTSTRING"))
            {
                printf("%s\n", gt->strval);
            }
            else if (!strcmp(gt->name, "CONSTINTD") || !strcmp(gt->name, "CONSTINTH"))
            {
                printf("%d\n", gt->intval);
            }
            else if (!strcmp(gt->name, "CONSTFLOAT"))
            {
                printf("%f\n", gt->floatval);
            }
            else if (!strcmp(gt->name, "CONSTCHAR"))
            {
                printf("'%c'\n", gt->charval);
            }
            else
            {
                printf("(%d)\n", gt->line);
            }
        }
        else
        {
            printf("%s  (null)\n", gt->name);
        }

        traverseTree(gt->lchild, level + 1);
        traverseTree(gt->rchild, level);
    }
}

void yyerror(const char *s, ...) //变长参数错误处理函数
{
    va_list ap;
    va_start(ap,s);
    fprintf(stderr, "Line:%d Error:", yylineno);//错误行号
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        if (!(yyin = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return 1;
        }

        if (wordparse)
        {
            printf("\n词 法 分 析 ：\n");
        }
        yyparse();
    }
    return 0;
}
