#ifndef ENUM_H
#define ENUM_H

// define node type
typedef enum nodeType {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STR,    //string constance
    TOKEN_ID,
    TOKEN_TYPE,
    TOKEN_OTHER,
    NOT_A_TOKEN
} NodeType;

//define 4 kind variabal format 
typedef enum _kind { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;

/*
define all base type
guarantee scalability

*/
typedef enum _basicType { INT_TYPE, FLOAT_TYPE,STR_TYPE } BasicType;







#endif