/**
 * Author: starkwj
 * Created on: 2018-05-24
 * Modified on: 2018-05-29
 */
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "grammartree.h"
    #include "grammartree.tab.h"
    int wordparse = 0;
%}

%option yylineno

COMMA           [,]
SEMICOLEN       [;]
LEFTBRAKET      [(]
RIGHTBRAKET     [)]
LEFTBRACE       [{]
RIGHTBRACE      [}]

OPASSIGN        [=]
OPADD           [+]
OPMINUS         [-]
OPMULTIPLY      [*]
OPDIVIDE        [/]
OPADDASSIGN     "+="
OPMINUSASSIGN   "-="
OPMULASSIGN     "*="
OPDIVASSIGN     "/="
OPSELFADD       "++"
OPSELFMINUS     "--"

ROPAND          [&&]
ROPOR           [||]
ROPNOT          [!]
RELOP           >|<|>=|<=|==|!=

TYPE            void|bool|char|int|float|string

KEYIF           if
KEYELSE         else
KEYWHILE        while
KEYFOR          for
KEYRETURN       return
KEYPRINT        print

CONSTBOOL       false|true
CONSTCHAR       \'.\'
CONSTINTD       [+-]?[0-9]+
CONSTINTH       [+-]?0[xX][0-9a-fA-f]+
CONSTFLOAT      [+-]?([0-9]*\.?[0-9]+|[0-9]+\.)[f]?
CONSTSTRING     \"[^\"\n]*\"
IDENTIFIER      [a-zA-Z_][a-zA-Z0-9_]*

COMMENT         (\/\/.*)|(\/\*((\*[^\/]?)|[^\*]*)*\*\/)

DELIM           [ \t\n]
NLINE           [\n]
WS              {DELIM}+

WRONGIDENTIFIER [0-9][a-zA-Z0-9_]*
WRONGSTRING     \"[^\"\n]*$
AERROR          .

%%
{COMMA} {
    if (wordparse) {
        printf("(%s, COMMA)\n", yytext);
    }
    yylval.grammartree = createTree("COMMA", 0, yylineno);
    return COMMA;
}
{SEMICOLEN} {
    if (wordparse) {
        printf("(%s, SEMICOLEN)\n", yytext);
    }
    yylval.grammartree = createTree("SEMICOLEN", 0, yylineno);
    return SEMICOLEN;
}
{LEFTBRAKET} {
    if (wordparse) {
        printf("(%s, LEFTBRAKET)\n", yytext);
    }
    yylval.grammartree = createTree("LEFTBRAKET", 0, yylineno);
    return LEFTBRAKET;
}
{RIGHTBRAKET} {
    if (wordparse) {
        printf("(%s, RIGHTBRAKET)\n", yytext);
    }
    yylval.grammartree = createTree("RIGHTBRAKET", 0, yylineno);
    return RIGHTBRAKET;
}
{LEFTBRACE} {
    if (wordparse) {
        printf("(%s, LEFTBRACE)\n", yytext);
    }
    yylval.grammartree = createTree("LEFTBRACE", 0, yylineno);
    return LEFTBRACE;
}
{RIGHTBRACE} {
    if (wordparse) {
        printf("(%s, RIGHTBRACE)\n", yytext);
    }
    yylval.grammartree = createTree("RIGHTBRACE", 0, yylineno);
    return RIGHTBRACE;
}
{OPASSIGN} {
    if (wordparse) {
        printf("(%s, OPASSIGN)\n", yytext);
    }
    yylval.grammartree = createTree("OPASSIGN", 0, yylineno);
    return OPASSIGN;
}
{OPADD} {
    if (wordparse) {
        printf("(%s, OPADD)\n", yytext);
    }
    yylval.grammartree = createTree("OPADD", 0, yylineno);
    return OPADD;
}
{OPMINUS} {
    if (wordparse) {
        printf("(%s, OPMINUS)\n", yytext);
    }
    yylval.grammartree = createTree("OPMINUS", 0, yylineno);
    return OPMINUS;
}
{OPMULTIPLY} {
    if (wordparse) {
        printf("(%s, OPMULTIPLY)\n", yytext);
    }
    yylval.grammartree = createTree("OPMULTIPLY", 0, yylineno);
    return OPMULTIPLY;
}
{OPDIVIDE} {
    if (wordparse) {
        printf("(%s, OPDIVIDE)\n", yytext);
    }
    yylval.grammartree = createTree("OPDIVIDE", 0, yylineno);
    return OPDIVIDE;
}
{OPADDASSIGN} {
    if (wordparse) {
        printf("(%s, OPADDASSIGN)\n", yytext);
    }
    yylval.grammartree = createTree("OPADDASSIGN", 0, yylineno);
    return OPADDASSIGN;
}
{OPMINUSASSIGN} {
    if (wordparse) {
        printf("(%s, OPMINUSASSIGN)\n", yytext);
    }
    yylval.grammartree = createTree("OPMINUSASSIGN", 0, yylineno);
    return OPMINUSASSIGN;
}
{OPMULASSIGN} {
    if (wordparse) {
        printf("(%s, OPMULASSIGN)\n", yytext);
    }
    yylval.grammartree = createTree("OPMULASSIGN", 0, yylineno);
    return OPMULASSIGN;
}
{OPDIVASSIGN} {
    if (wordparse) {
        printf("(%s, OPDIVASSIGN)\n", yytext);
    }
    yylval.grammartree = createTree("OPDIVASSIGN", 0, yylineno);
    return OPDIVASSIGN;
}
{OPSELFADD} {
    if (wordparse) {
        printf("(%s, OPSELFADD)\n", yytext);
    }
    yylval.grammartree = createTree("OPSELFADD", 0, yylineno);
    return OPSELFADD;
}
{OPSELFMINUS} {
    if (wordparse) {
        printf("(%s, OPSELFMINUS)\n", yytext);
    }
    yylval.grammartree = createTree("OPSELFMINUS", 0, yylineno);
    return OPSELFMINUS;
}
{ROPAND} {
    if (wordparse) {
        printf("(%s, ROPAND)\n", yytext);
    }
    yylval.grammartree = createTree("ROPAND", 0, yylineno);
    return ROPAND;
}
{ROPOR} {
    if (wordparse) {
        printf("(%s, ROPOR)\n", yytext);
    }
    yylval.grammartree = createTree("ROPOR", 0, yylineno);
    return ROPOR;
}
{ROPNOT} {
    if (wordparse) {
        printf("(%s, ROPNOT)\n", yytext);
    }
    yylval.grammartree = createTree("ROPNOT", 0, yylineno);
    return ROPNOT;
}
{RELOP} {
    if (wordparse) {
        printf("(%s, RELOP)\n", yytext);
    }
    yylval.grammartree = createTree("RELOP", 0, yylineno);
    return RELOP;
}
{TYPE} {
    if (wordparse) {
        printf("(%s, TYPE)\n", yytext);
    }
    yylval.grammartree = createTree("TYPE", 0, yylineno);
    return TYPE;
}
{KEYIF} {
    if (wordparse) {
        printf("(%s, KEYIF)\n", yytext);
    }
    yylval.grammartree = createTree("KEYIF", 0, yylineno);
    return KEYIF;
}
{KEYELSE} {
    if (wordparse) {
        printf("(%s, KEYELSE)\n", yytext);
    }
    yylval.grammartree = createTree("KEYELSE", 0, yylineno);
    return KEYELSE;
}
{KEYWHILE} {
    if (wordparse) {
        printf("(%s, KEYWHILE)\n", yytext);
    }
    yylval.grammartree = createTree("KEYWHILE", 0, yylineno);
    return KEYWHILE;
}
{KEYFOR} {
    if (wordparse) {
        printf("(%s, KEYFOR)\n", yytext);
    }
    yylval.grammartree = createTree("KEYFOR", 0, yylineno);
    return KEYFOR;
}
{KEYRETURN} {
    if (wordparse) {
        printf("(%s, KEYRETURN)\n", yytext);
    }
    yylval.grammartree = createTree("KEYRETURN", 0, yylineno);
    return KEYRETURN;
}
{KEYPRINT} {
    if (wordparse) {
        printf("(%s, KEYPRINT)\n", yytext);
    }
    yylval.grammartree = createTree("KEYPRINT", 0, yylineno);
    return KEYPRINT;
}
{CONSTBOOL} {
    if (wordparse) {
        printf("(%s, CONSTBOOL)\n", yytext);
    }
    yylval.grammartree = createTree("CONSTBOOL", 0, yylineno);
    return CONSTBOOL;
}
{CONSTCHAR} {
    if (wordparse) {
        printf("(%s, CONSTCHAR)\n", yytext);
    }
    yylval.grammartree = createTree("CONSTCHAR", 0, yylineno);
    return CONSTCHAR;
}
{CONSTINTD} {
    if (wordparse) {
        printf("(%s, CONSTINTD)\n", yytext);
    }
    yylval.grammartree = createTree("CONSTINTD", 0, yylineno);
    return CONSTINTD;
}
{CONSTINTH} {
    if (wordparse) {
        printf("(%s, CONSTINTH)\n", yytext);
    }
    yylval.grammartree = createTree("CONSTINTH", 0, yylineno);
    return CONSTINTH;
}
{CONSTFLOAT} {
    if (wordparse) {
        printf("(%s, CONSTFLOAT)\n", yytext);
    }
    yylval.grammartree = createTree("CONSTFLOAT", 0, yylineno);
    return CONSTFLOAT;
}
{CONSTSTRING} {
    if (wordparse) {
        printf("(%s, CONSTSTRING)\n", yytext);
    }
    yylval.grammartree = createTree("CONSTSTRING", 0, yylineno);
    return CONSTSTRING;
}
{IDENTIFIER} {
    if (wordparse) {
        printf("(%s, IDENTIFIER)\n", yytext);
    }
    yylval.grammartree = createTree("IDENTIFIER", 0, yylineno);
    return IDENTIFIER;
}
{COMMENT} {
    if (wordparse) {
        printf("(%s, COMMENT)\n", yytext);
    }
}
{WS} {
    /* ignore */
}
{WRONGIDENTIFIER} {
    printf("Error type at Line %d: Wrong format of identifier '%s'.\n", yylineno, yytext);
    exit(-1);
}
{AERROR} {
    printf("Error type at Line %d: Unrecognized characters.\n", yylineno);
    exit(-1);
}

%%
int yywrap() {
    return 1;
}
