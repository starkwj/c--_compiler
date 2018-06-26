/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_GRAMMARTREE_TAB_H_INCLUDED
# define YY_YY_GRAMMARTREE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    COMMA = 258,
    SEMICOLEN = 259,
    LEFTBRAKET = 260,
    RIGHTBRAKET = 261,
    LEFTBRACE = 262,
    RIGHTBRACE = 263,
    OPASSIGN = 264,
    OPADD = 265,
    OPMINUS = 266,
    OPSELFADD = 267,
    OPSELFMINUS = 268,
    OPMULTIPLY = 269,
    OPDIVIDE = 270,
    OPADDASSIGN = 271,
    OPMINUSASSIGN = 272,
    OPMULASSIGN = 273,
    OPDIVASSIGN = 274,
    ROPAND = 275,
    ROPOR = 276,
    ROPNOT = 277,
    RELOP = 278,
    TYPE = 279,
    KEYIF = 280,
    KEYELSE = 281,
    KEYWHILE = 282,
    KEYFOR = 283,
    KEYRETURN = 284,
    KEYPRINT = 285,
    NLINE = 286,
    CONSTBOOL = 287,
    CONSTCHAR = 288,
    CONSTINTD = 289,
    CONSTINTH = 290,
    CONSTFLOAT = 291,
    CONSTSTRING = 292,
    IDENTIFIER = 293,
    UMINUS = 294,
    UADD = 295,
    NOELSE = 296
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 19 "grammartree.y" /* yacc.c:1909  */

    GTNODE * grammartree;

#line 100 "grammartree.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_GRAMMARTREE_TAB_H_INCLUDED  */
