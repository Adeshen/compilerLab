
#ifndef INTER_STRUCT_H
#define INTER_STRUCT_H

#include "ASTNodes.hpp"
#include "semantic_symbol_struct.h"
#include "enum.hpp"

typedef struct _operand* pOperand;
typedef struct _interCode* pInterCode;
typedef struct _interCodes* pInterCodes;
typedef struct _arg* pArg;
typedef struct _argList* pArgList;
typedef struct _interCodeList* pInterCodeList;

typedef struct _operand {

    OpKind kind;

    union {
        int value;
        char* name;
    } u;

    // boolean isAddr;
} Operand;

typedef struct _interCode {
    IrKind kind;
    union {
        struct {
            pOperand op;
        } oneOp;
        struct {
            pOperand right, left;
        } assign;
        struct {
            pOperand result, op1, op2;
        } binOp;
        struct {
            pOperand x, relop, y, z;
        } ifGoto;
        struct {
            pOperand op;
            int size;
        } dec;
    } u;
} InterCode;

typedef struct _interCodes {
    pInterCode code;
    pInterCodes prev, next;
} InterCodes;

typedef struct _arg {
    pOperand op;
    pArg next;
} Arg;

typedef struct _argList {
    pArg head;
    pArg cur;
} ArgList;

typedef struct _interCodeList {
    pInterCodes head;
    pInterCodes cur;
    char* lastArrayName;  // 针对结构体数组，因为需要数组名查表
    int tempVarNum;
    int labelNum;
} InterCodeList;

extern bool interError;
extern pInterCodeList interCodeList;

// Operand func
pOperand newOperand(int kind, ...);
void deleteOperand(pOperand p);
void setOperand(pOperand p, int kind, void* val);
void printOp(FILE* fp, pOperand op);

// InterCode func
pInterCode newInterCode(int kind, ...);
void deleteInterCode(pInterCode p);
void printInterCode(FILE* fp, pInterCodeList interCodeList);

// InterCodes func
pInterCodes newInterCodes(pInterCode code);
void deleteInterCodes(pInterCodes p);

// Arg and ArgList func
pArg newArg(pOperand op);
pArgList newArgList();
void deleteArg(pArg p);
void deleteArgList(pArgList p);
void addArg(pArgList argList, pArg arg);

// InterCodeList func
pInterCodeList newInterCodeList();
void deleteInterCodeList(pInterCodeList p);
void addInterCode(pInterCodeList interCodeList, pInterCodes newCode);


// traverse func
pOperand newTemp();
pOperand newLabel();
int getSize(pType type);
#endif
