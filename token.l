%{ /* -*- C++ -*- */
# include<stdio.h>
# include <string>
# include "ASTNodes.hpp"
# include "parser.hpp"
#define TOKEN_NODE(node_type,name) yylval.node = newTokenNode(yylineno,node_type,(char*)name, yytext);
#define SAVE_TOKEN yylval.string = new std::string(yytext)
//#define TOKEN(t) ( yylval.token = t)

extern bool lexError;

extern YYLTYPE yylloc;

int lines = 0;
int yycolumn = 1;


#define YY_USER_ACTION \
    yylloc.first_line=yylloc.last_line=yylineno; \
    yylloc.first_column=yycolumn; \
    yylloc.last_column=yycolumn+yyleng-1; \
    yycolumn+=yyleng;

%}

%option yylineno

%option noyywrap 


TYPE int|float|char|bool|string|void
RELOP 	>|<|>=|<=|==|!=

%%
"//".*                     //printf("dd注释\n");
"/*"(\\.|[^"*/"])*"*/"     //  printf("注释\n");
[ \t\r\n]				   ;

"if"                     TOKEN_NODE(TOKEN_OTHER,"IF");return TIF;
"else"                   TOKEN_NODE(TOKEN_OTHER,"ELSE");return TELSE;
"return"                 TOKEN_NODE(TOKEN_OTHER,"RETURN");return TRETURN;
"for"                    TOKEN_NODE(TOKEN_OTHER,"RETURN");return TFOR;
"while"                  TOKEN_NODE(TOKEN_OTHER,"THILE");return TWHILE;
"struct"                 TOKEN_NODE(TOKEN_OTHER,"struct");return TSTRUCT;
{TYPE}                   TOKEN_NODE(TOKEN_TYPE,"TYPE");return TYPE;
"extern"                 TOKEN_NODE(TOKEN_OTHER,"EXTERN");return TEXTERN;
{RELOP}                  {TOKEN_NODE(TOKEN_OTHER, "RELOP"); return RELOP; }

[a-zA-Z_][a-zA-Z0-9_]*	          TOKEN_NODE(TOKEN_ID,"ID");return TIDENTIFIER;
[0-9]+\.[0-9]*	                  TOKEN_NODE(TOKEN_FLOAT,"FLOAT");return TDOUBLE;
[+-]?[0-9]+[Ee][+-]?[0-9]+       TOKEN_NODE(TOKEN_FLOAT,"FLOAT");return TDOUBLE;
[-+]?[0-9]+\.[0-9]*[Ee][+-]?[0-9]+   TOKEN_NODE(TOKEN_FLOAT,"FLOAT");return TDOUBLE;
0|[1-9][0-9]*  				      TOKEN_NODE(TOKEN_INT,"INT");return TINTEGER;
0[xX][0-9a-fA-F]+                 TOKEN_NODE(TOKEN_INT,"INT");return TINTEGER;
0[1-7][0-7]*                      TOKEN_NODE(TOKEN_INT,"INT");return TINTEGER;
\"(\\.|[^"])*\"                   TOKEN_NODE(TOKEN_STR,"STR");return TLITERAL;

"="						 TOKEN_NODE(TOKEN_OTHER,"ASSIGNOP");return TEQUAL;

"("                      TOKEN_NODE(TOKEN_OTHER,"LP");return TLPAREN;
")"                      TOKEN_NODE(TOKEN_OTHER,"RP");return TRPAREN;
"{"                      TOKEN_NODE(TOKEN_OTHER,"LC");return TLBRACE;
"}"                      TOKEN_NODE(TOKEN_OTHER,"RC");return TRBRACE;
"["                      TOKEN_NODE(TOKEN_OTHER,"LB");return TLBRACKET;
"]"                      TOKEN_NODE(TOKEN_OTHER,"RB");return TRBRACKET;

"."                      TOKEN_NODE(TOKEN_OTHER,"DOT");return TDOT;
","                      TOKEN_NODE(TOKEN_OTHER,"COMMA");return TCOMMA;

"+"                      TOKEN_NODE(TOKEN_OTHER,"PLUS");return TPLUS;
"-"                      TOKEN_NODE(TOKEN_OTHER,"MINUS");return TMINUS;
"*"                      TOKEN_NODE(TOKEN_OTHER,"STAR");return TSTAR;
"/"                      TOKEN_NODE(TOKEN_OTHER,"DIV");return TDIV;
"&"                      TOKEN_NODE(TOKEN_OTHER,"AND");return TAND;
"|"                      TOKEN_NODE(TOKEN_OTHER,"OR");return TOR;
"!"                      TOKEN_NODE(TOKEN_OTHER,"NOT");return TNOT;
"^"                      TOKEN_NODE(TOKEN_OTHER,"XOR");return TXOR;
"%"                      TOKEN_NODE(TOKEN_OTHER,"MOD");return TMOD;
">>"                     TOKEN_NODE(TOKEN_OTHER,"SHIFTR");return TSHIFTR;
"<<"                     TOKEN_NODE(TOKEN_OTHER,"SHIFTL");return TSHIFTL;
";"                      TOKEN_NODE(TOKEN_OTHER,"SEMI");return TSEMICOLON;


[0-7]*[9][0-7]*                              lexError=1;printf("Error type A at Line %d:  Illegal octal number “%s”\n",yylineno,yytext);return TIDENTIFIER;
0[xX][0-9a-fA-F]*[g-zG-Z][0-9a-fA-F]*        lexError=1;printf("Error type A at Line %d:  Illegal hexadecimal number “%s”\n",yylineno,yytext);return TIDENTIFIER;
[-+]?[0-9]+\.[0-9]*[Ee]                      lexError=1;printf("Error type A at Line %d:  Illegal floating point number “%s”\n",yylineno,yytext);return TDOUBLE;
.						                     lexError=1;printf("Error type A at Line %d:  Mysterious charactrer “%s”\n",yylineno,yytext);//yyterminate();

%%







