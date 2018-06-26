/**
 * Author: starkwj
 * Created on: 2018-05-29
 */
%{
    #include <stdio.h>
    #include "grammartree.h"
    #include "lex.yy.c"
    #include "symbol.h"
    int haserror = 0;
    int serror = 0;
    int printTable = 1;
%}

%error-verbose

%union {
    GTNODE * grammartree;
}
/* declare tokens */
%token <grammartree> COMMA SEMICOLEN LEFTBRAKET RIGHTBRAKET LEFTBRACE RIGHTBRACE
%token <grammartree> OPASSIGN OPADD OPMINUS OPSELFADD OPSELFMINUS OPMULTIPLY OPDIVIDE
%token <grammartree> OPADDASSIGN OPMINUSASSIGN OPMULASSIGN OPDIVASSIGN
%token <grammartree> ROPAND ROPOR ROPNOT RELOP
%token <grammartree> TYPE
%token <grammartree> KEYIF KEYELSE KEYWHILE KEYFOR KEYRETURN KEYPRINT NLINE
%token <grammartree> CONSTBOOL CONSTCHAR CONSTINTD CONSTINTH CONSTFLOAT CONSTSTRING IDENTIFIER

%type <grammartree> Program ExtDefList ExtDef ExtDecList FunDec VarList ParamDec
CompSt StmtList Stmt IfStmt ForStmt WhileStmt ReturnStmt PrintStmt DefList Def DecList Dec
Exp CONSTVALUE Args

/* priority */
%right OPASSIGN OPADDASSIGN OPMINUSASSIGN OPMULASSIGN OPDIVASSIGN
%left ROPOR
%left ROPAND
%left RELOP
%left OPADD OPMINUS
%left OPMULTIPLY OPDIVIDE
%left UMINUS UADD
%right ROPNOT OPSELFADD OPSELFMINUS
%left LEFTBRACE RIGHTBRACE LEFTBRAKET RIGHTBRAKET

%nonassoc NOELSE
%nonassoc KEYELSE

%%
Program: ExtDefList {
        $$ = createTree("Program", 1, $1);
        if (!haserror) {
            createSymbolTable($$, 1, 0);
            removeSymbolOfLevel(1);
            if (!serror && printTable) {
                createSymbolTable($$, 1, 1);
            }
        }
    }
    ;
ExtDefList: ExtDef ExtDefList {$$ = createTree("ExtDefList", 2, $1, $2);}
    | {$$ = createTree("ExtDefList", 0, -1);}
    ;
ExtDef: TYPE ExtDecList SEMICOLEN {$$ = createTree("ExtDef", 3, $1, $2, $3);}
    | TYPE FunDec CompSt {$$ = createTree("ExtDef", 3, $1, $2, $3);}
    | error SEMICOLEN {$$ = createTree("ExtDef", 1, $2); haserror = 1;}
    | error RIGHTBRACE {$$ = createTree("ExtDef", 1, $2); haserror = 1;}
    ;
ExtDecList: IDENTIFIER {$$ = createTree("ExtDecList", 1, $1);}
    | IDENTIFIER OPASSIGN CONSTVALUE {$$ = createTree("ExtDecList", 3, $1, $2, $3);}
    | IDENTIFIER COMMA ExtDecList {$$ = createTree("ExtDecList", 3, $1, $2, $3);}
    | IDENTIFIER OPASSIGN CONSTVALUE COMMA ExtDecList {$$ = createTree("ExtDecList", 5, $1, $2, $3, $4, $5);}
    ;

FunDec: IDENTIFIER LEFTBRAKET VarList RIGHTBRAKET {$$ = createTree("FunDec", 4, $1, $2, $3, $4);}
    | IDENTIFIER LEFTBRAKET RIGHTBRAKET {$$ = createTree("FunDec", 3, $1, $2, $3);}
    ;
VarList: ParamDec COMMA VarList {$$ = createTree("VarList", 3, $1, $2, $3);}
    | ParamDec {$$ = createTree("VarList", 1, $1);}
    ;
ParamDec: TYPE IDENTIFIER {$$ = createTree("ParamDec", 2, $1, $2);}
    ;

CompSt: LEFTBRACE DefList StmtList RIGHTBRACE {$$ = createTree("CompSt", 4, $1, $2, $3, $4);}
    ;
StmtList: Stmt StmtList {$$ = createTree("StmtList", 2, $1, $2);}
    | {$$ = createTree("StmtList", 0, -1);}
    ;
Stmt: Exp SEMICOLEN {$$ = createTree("Stmt", 2, $1, $2);}
    | CompSt {$$ = createTree("Stmt", 1, $1);}
    | IfStmt {$$ = createTree("Stmt", 1, $1);}
    | ForStmt {$$ = createTree("Stmt", 1, $1);}
    | WhileStmt {$$ = createTree("Stmt", 1, $1);}
    | ReturnStmt SEMICOLEN {$$ = createTree("Stmt", 2, $1, $2);}
    | PrintStmt SEMICOLEN {$$ = createTree("Stmt", 2, $1, $2);}
    | error SEMICOLEN {$$ = createTree("Stmt", 1, $2); haserror = 1;}
    ;
IfStmt: KEYIF LEFTBRAKET Exp RIGHTBRAKET Stmt %prec NOELSE {$$ = createTree("IfStmt", 5, $1, $2, $3, $4, $5);}
    | KEYIF LEFTBRAKET Exp RIGHTBRAKET Stmt KEYELSE Stmt {$$ = createTree("IfStmt", 7, $1, $2, $3, $4, $5, $6, $7);}
    ;
ForStmt: KEYFOR LEFTBRAKET Exp SEMICOLEN Exp SEMICOLEN Exp RIGHTBRAKET Stmt {$$ = createTree("ForStmt", 9, $1, $2, $3, $4, $5, $6, $7, $8, $9);}
    ;
WhileStmt: KEYWHILE LEFTBRAKET Exp RIGHTBRAKET Stmt {$$ = createTree("WhileStmt", 5, $1, $2, $3, $4, $5);}
    ;
ReturnStmt: KEYRETURN {$$ = createTree("ReturnStmt", 1, $1);}
    | KEYRETURN Exp {$$ = createTree("ReturnStmt", 2, $1, $2);}
    ;
PrintStmt: KEYPRINT LEFTBRAKET Exp RIGHTBRAKET {$$ = createTree("PrintStmt", 4, $1, $2, $3, $4);}
    ;

DefList: Def DefList {$$ = createTree("DefList", 2, $1, $2);}
    | {$$ = createTree("DefList", 0, -1);}
    ;
Def: TYPE DecList SEMICOLEN {$$ = createTree("Def", 3, $1, $2, $3);}
    ;
DecList: Dec {$$ = createTree("DecList", 1, $1);}
    | Dec COMMA DecList {$$ = createTree("DecList", 3, $1, $2, $3);}
    ;
Dec: IDENTIFIER {$$ = createTree("Dec", 1, $1);}
    | IDENTIFIER OPASSIGN Exp {$$ = createTree("Dec", 3, $1, $2, $3);}
    ;

Exp: Exp OPASSIGN Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp ROPAND Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp ROPOR Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp RELOP Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPADD Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPMINUS Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPMULTIPLY Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPDIVIDE Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPADDASSIGN Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPMINUSASSIGN Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPMULASSIGN Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | Exp OPDIVASSIGN Exp {$$ = createTree("Exp", 3, $1, $2, $3);}
    | IDENTIFIER OPSELFADD {$$ = createTree("Exp", 2, $1, $2);}
    | IDENTIFIER OPSELFMINUS {$$ = createTree("Exp", 2, $1, $2);}
    | OPSELFADD IDENTIFIER {$$ = createTree("Exp", 2, $1, $2);}
    | OPSELFMINUS IDENTIFIER {$$ = createTree("Exp", 2, $1, $2);}
    | LEFTBRAKET Exp RIGHTBRAKET {$$ = createTree("Exp", 3, $1, $2, $3);}
    | ROPNOT Exp {$$ = createTree("Exp", 2, $1, $2);}
    | OPMINUS Exp %prec UMINUS {$$ = createTree("Exp", 2, $1, $2);}
    | OPADD Exp %prec UADD {$$ = createTree("Exp", 2, $1, $2);}
    | IDENTIFIER LEFTBRAKET Args RIGHTBRAKET {$$ = createTree("Exp", 4, $1, $2, $3, $4);}
    | IDENTIFIER LEFTBRAKET RIGHTBRAKET {$$ = createTree("Exp", 3, $1, $2, $3);}
    | IDENTIFIER {$$ = createTree("Exp", 1, $1);}
    | CONSTVALUE {$$ = createTree("Exp", 1, $1);}
    ;
CONSTVALUE: CONSTBOOL {$$ = createTree("CONSTVALUE", 1, $1);}
    | CONSTCHAR {$$ = createTree("CONSTVALUE", 1, $1);}
    | CONSTINTD {$$ = createTree("CONSTVALUE", 1, $1);}
    | CONSTINTH {$$ = createTree("CONSTVALUE", 1, $1);}
    | CONSTFLOAT {$$ = createTree("CONSTVALUE", 1, $1);}
    | CONSTSTRING {$$ = createTree("CONSTVALUE", 1, $1);}
    ;
Args: Exp COMMA Args {$$ = createTree("Args", 3, $1, $2, $3);}
    | Exp {$$ = createTree("Args", 1, $1);}
    ;

