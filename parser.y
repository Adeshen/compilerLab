%require "3.5.1"

%code requires {
#include<iostream>
#include <string>
#include "ASTNodes.hpp"
extern int yylineno;
extern int yylex (void);
int yyerror (const char *msg);
//void yyerror (YYLTYPE *locp, char const *msg);



extern bool synError;
extern pNode root;


}


//%locations
//%define api.pure full
//%define api.location.type full

//%define api.location.type {location_t}
%code requires {

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

}


%union{
    std::string* string;

    pNode node;
    int token;
    
}


%token <node> TIDENTIFIER TINTEGER TDOUBLE TYPE TEXTERN TLITERAL 
%token <node> RELOP TEQUAL
%token <node> TLPAREN TRPAREN TLBRACE TRBRACE TCOMMA TDOT TSEMICOLON TLBRACKET TRBRACKET TQUOTATION
%token <node> TPLUS TMINUS TSTAR TDIV TAND TOR  TXOR TMOD TNEG TNOT TSHIFTL TSHIFTR
%token <node> TIF TELSE TFOR TWHILE TRETURN TSTRUCT


%type <node> Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type <node> Specifier StructSpecifier OptTag Tag   //  Specifiers
%type <node> VarDec FunDec VarList ParamDec         //  Declarators
%type <node> CompSt StmtList Stmt                   //  Statements
%type <node> DefList Def Dec DecList                //  Local Definitions
%type <node> Exp Args                               //  Expressions



%right TEQUAL
%left TOR
%left TAND
%left TPLUS TMINUS
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE



%start Program

%%

    
    
    Program : ExtDefList                          { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Program", 1, $1); root = $$; }
    ;
    ExtDefList : ExtDef ExtDefList                { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"ExtDefList", 2, $1, $2); }
        |                                         { $$ = NULL; } 
    ;
    ExtDef : Specifier ExtDecList TSEMICOLON      { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"ExtDef", 3, $1, $2, $3); }
        | Specifier TSEMICOLON                    { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"ExtDef", 2, $1,$2); }
        | Specifier FunDec CompSt                 { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"ExtDef", 3, $1, $2, $3); }
        | error TSEMICOLON                              { synError = true; }
    ;
    ExtDecList : VarDec              { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"ExtDecList", 1, $1); }
        | VarDec TCOMMA ExtDecList   { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"ExtDecList", 3, $1, $2, $3); }
    ;


    Specifier : TYPE                  { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Specifier", 1, $1); }
            | StructSpecifier         { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Specifier", 1, $1); }
    ;

    StructSpecifier : TSTRUCT OptTag TLBRACE DefList TRBRACE   { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"StructSpecifier", 5, $1, $2, $3, $4, $5); }
            | TSTRUCT Tag       { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"StructSpecifier", 2, $1, $2); }
    ;

    OptTag : TIDENTIFIER        { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"OptTag", 1, $1); }
            |                   { $$ = NULL; }
    ;

    Tag : TIDENTIFIER           { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Tag", 1, $1); }
    ;



    VarDec : TIDENTIFIER                               { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"VarDec", 1, $1); }
            |  VarDec TLBRACKET TINTEGER TRBRACKET     { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"VarDec", 4, $1, $2, $3, $4); }
            |  error RB                                { synError = true; }
    ;

    FunDec : TIDENTIFIER TLPAREN VarList TRPAREN         { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"FunDec", 4, $1, $2, $3, $4); }
            | TIDENTIFIER TLPAREN TRPAREN                { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"FunDec", 3, $1, $2, $3); }
            | error RP                                   { synError = true; }
    ;

    VarList : ParamDec TCOMMA VarList                    { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"VarList", 3, $1, $2, $3); }
            | ParamDec                                   { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"VarList", 1, $1); }
    ;

    ParamDec : Specifier VarDec                          { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"ParamDec", 2, $1, $2); }
    ;



    CompSt : TLBRACE DefList StmtList TRBRACE           { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"CompSt", 4, $1, $2, $3, $4); }
            | error TRBRACE                                  { synError = true; }
    ;
    StmtList : Stmt StmtList                            { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"StmtList", 2, $1, $2); }
            |                                           { $$ = NULL; }
    ;
    Stmt : Exp TSEMICOLON                               { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Stmt", 2, $1, $2); }
        | CompSt                                        { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Stmt", 1, $1); }
        | TRETURN Exp TSEMICOLON                        { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Stmt", 3, $1, $2, $3); } 
        | TIF TLPAREN Exp TRPAREN Stmt                  { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Stmt", 5, $1, $2, $3, $4, $5); }
        | TIF TLPAREN Exp TRPAREN Stmt TELSE Stmt       { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
        | TWHILE TLPAREN Exp TRPAREN Stmt               { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Stmt", 5, $1, $2, $3, $4, $5); }
        | error TSEMICOLON                              { synError = true; }
    ;


    DefList : Def DefList                       { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"DefList", 2, $1, $2); }
        |                                       { $$ = NULL; }
    ;
    Def : Specifier DecList TSEMICOLON          { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Def", 3, $1, $2, $3); }
    ;
    DecList : Dec                               { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"DecList", 1,$1); }
        | Dec TCOMMA DecList                    { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"DecList", 3, $1, $2, $3); }
    ;
    Dec : VarDec                                { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Dec", 1, $1); }
        | VarDec TEQUAL Exp                     { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Dec", 3, $1, $2, $3); }
    ;



    Exp : Exp TEQUAL Exp                        { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp TAND Exp                          { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp TOR Exp                           { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp RELOP Exp                         { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp TPLUS Exp                         { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp TMINUS Exp                        { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp TSTAR Exp                         { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp TDIV Exp                          { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | TLPAREN Exp TRPAREN                   { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | TMINUS Exp                            { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 2, $1, $2); }
        | TNOT Exp                              { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 2, $1, $2); }
        | TIDENTIFIER TLPAREN Args TRPAREN      { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 4, $1, $2, $3, $4); }
        | TIDENTIFIER TLPAREN TRPAREN           { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3); }
        | Exp TLBRACKET Exp TRBRACKET   { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 4, $1, $2, $3, $4); }
        | Exp TDOT TIDENTIFIER          { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 3, $1, $2, $3);}
        | TIDENTIFIER                   { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 1, $1); }
        | TINTEGER                      { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 1, $1); }
        | TDOUBLE                       { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Exp", 1, $1); }
    ;
    Args : Exp TCOMMA Args              { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Args", 3, $1, $2, $3); }
        | Exp                           { $$ = newNode(@$.first_line, NOT_A_TOKEN, (char*)"Args", 1, $1); }
    ;



%%




int  yyerror(const char* msg){
    fprintf(stderr, "Error type B at line %d: %s.\n",yylineno, msg);
    return 1;
}
