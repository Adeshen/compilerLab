/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 3 "parser.y"

#include<iostream>
#include <string>
#include "ASTNodes.hpp"
extern int yylineno;
extern int yylex (void);
int yyerror (const char *msg);
//void yyerror (YYLTYPE *locp, char const *msg);



extern bool synError;
extern pNode root;


#line 26 "parser.y"


/*#define YYLTYPE YYLTYPE
    typedef struct YYLTYPE
    {
        int first_line;
        int first_column;
        int last_line;
        int last_column;
        char *filename;
    } YYLTYPE;

*/


#line 80 "parser.hpp"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TIDENTIFIER = 258,
    TINTEGER = 259,
    TDOUBLE = 260,
    TYPE = 261,
    TEXTERN = 262,
    TLITERAL = 263,
    RELOP = 264,
    TEQUAL = 265,
    TLPAREN = 266,
    TRPAREN = 267,
    TLBRACE = 268,
    TRBRACE = 269,
    TCOMMA = 270,
    TDOT = 271,
    TSEMICOLON = 272,
    TLBRACKET = 273,
    TRBRACKET = 274,
    TQUOTATION = 275,
    TPLUS = 276,
    TMINUS = 277,
    TSTAR = 278,
    TDIV = 279,
    TAND = 280,
    TOR = 281,
    TXOR = 282,
    TMOD = 283,
    TNEG = 284,
    TNOT = 285,
    TSHIFTL = 286,
    TSHIFTR = 287,
    TIF = 288,
    TELSE = 289,
    TFOR = 290,
    TWHILE = 291,
    TRETURN = 292,
    TSTRUCT = 293,
    STAR = 294,
    DIV = 295,
    NOT = 296,
    DOT = 297,
    LB = 298,
    RB = 299,
    LP = 300,
    RP = 301,
    LOWER_THAN_ELSE = 302,
    ELSE = 303
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 43 "parser.y"

    std::string* string;

    pNode node;
    int token;
    

#line 148 "parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_PARSER_HPP_INCLUDED  */
