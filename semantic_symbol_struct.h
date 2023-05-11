#include "ASTNodes.hpp"
#include "enum.hpp"


#ifndef SEMANTIC_SYMBOL_STRUCT_H
#define SEMANTIC_SYMBOL_STRUCT_H

#define HASH_TABLE_SIZE 0x3fff

//字符串hash
//用于构建符号表
static inline unsigned int getHashCode(char* name) {
    unsigned int val = 0, i;
    // unsigned int x;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i= (val & ~HASH_TABLE_SIZE) >= 0)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    // x= (unsigned int)HASH_TABLE_SIZE;
    val %= (unsigned int)HASH_TABLE_SIZE;
    return val;
}

typedef struct type* pType;
typedef struct fieldList* pFieldList;
typedef struct tableItem* pItem;
typedef struct hashTable* pHash;
typedef struct stack* pStack;
typedef struct table* pTable;


typedef struct type {
    Kind kind;
    union {
        // 基本类型
        BasicType basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct {
            pType elem;
            int size;
        } array;
        // 结构体类型信息是一个链表
        struct {
            char* structName;
            pFieldList field;
        } structure;

        struct {
            int argc;          // argument counter
            pFieldList argv;   // argument vector
            pType returnType;  // returnType
            int hasDefined;
        } function;
    } u;
} Type;

//
typedef struct fieldList {
    char* name;       // 域的名字
    pType type;       // 域的类型
    pFieldList tail;  // 下一个域
    bool isArg;       // 实验3用，实验3中结构体参数需要传址，此处需要做特殊处理
} FieldList;

typedef struct tableItem {
    int symbolDepth;
    pFieldList field;
    pItem nextSymbol;  // same depth next symbol, linked from stack
    pItem nextHash;    // same hash code next symbol, linked from hash table
} TableItem;

typedef struct hashTable {
    pItem* hashArray;
} HashTable;

typedef struct stack {
    pItem* stackArray;
    int curStackDepth;
} Stack;

typedef struct table {
    pHash hash;
    pStack stack;
    int unNamedStructNum;
    // int enterStructLayer;
} Table;

extern pTable table;

//类型函数
pType newType(Kind kind, ...);
pType copyType(pType src);
void deleteType(pType type);
bool checkType(pType type1, pType type2);
void printType(pType type);
void *getTypeStr(pType p,char *str);

//成员链
pFieldList newFieldList(char* newName, pType newType);
pFieldList copyFieldList(pFieldList src);
void deleteFieldList(pFieldList fieldList);
void setFieldListName(pFieldList p, char* newName);
void printFieldList(pFieldList fieldList);
int getFieldListString(pFieldList fieldList,char *str);
int checkField(pFieldList a,pFieldList b);

//表项
pItem newItem(int symbolDepth, pFieldList pfield);
void deleteItem(pItem item);
bool isStructDef(pItem src);
bool isFuctionDef();


//hash 
pHash newHash();
void deleteHash(pHash hash);
pItem getHashHead(pHash hash, int index);
void setHashHead(pHash hash, int index, pItem newVal);

pStack newStack();

// Stack
void deleteStack(pStack stack);
void addStackDepth(pStack stack);
void minusStackDepth(pStack stack);
pItem getCurDepthStackHead(pStack stack);
void setCurDepthStackHead(pStack stack, pItem newVal);


// Table 
pTable initTable();
void deleteTable(pTable table);
pItem searchTableItem(pTable table, char* name);
bool checkTableItemConflict(pTable table, pItem item);
void addTableItem(pTable table, pItem item);
void deleteTableItem(pTable table, pItem item);
void clearCurDepthStackList(pTable table);

void printTable(pTable table);

void traverseTree(pNode node);


extern bool semError;
static inline void pError(ErrorType type, int line, char* msg) {
    semError=true;
    printf("Error type %d at Line %d: %s\n", type, line, msg);
}

#endif