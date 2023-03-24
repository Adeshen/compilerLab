#ifndef SEMANTIC_H
#define SEMENTIC_H


#include "ASTNodes.hpp"
#include "enum.hpp"
/*
    定义了符号表以及基本相应属性实现
*/

#define HASH_TABLE_SIZE 0x3fff

//字符串hash
//用于构建符号表
static inline unsigned int getHashCode(char* name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if (i = val & ~HASH_TABLE_SIZE)
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
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
        } function;
    } u;
} Type;








#endif