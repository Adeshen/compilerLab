#ifndef AST_Node_H
#define AST_Node_H 
#include<iostream>
#include<string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include "enum.hpp"

//#include "parser.hpp"

extern FILE *yyout;

typedef struct AST_node{
    int lineNo;  //  node in which line
    //   int depth;   //  node depth, for count white space for print
    NodeType type;  // node type
    char* name;     //  node name
    char* val;      //  node value

    struct AST_node* child;  //  non-terminals node first child node
    struct AST_node* next;   //  non-terminals node next brother node

} Node;

typedef Node* pNode;


/*
lineNo:   行标
NodeType: decide print logger format

*/
static inline pNode newNode(int lineNo,NodeType type,char *name,int argc,...)
{
    pNode curNode=NULL;
    int nameLength = strlen(name) + 1;
    curNode = (pNode)malloc(sizeof(Node));

    assert(curNode!=NULL);

    curNode->name = (char*) malloc(sizeof(char)*nameLength);

    assert(curNode->name!=NULL);

    curNode->lineNo=lineNo;

    curNode->type=type;

    strncpy(curNode->name,name,nameLength);

    va_list va_List;

    va_start(va_List,argc);

    pNode tempNode = va_arg(va_List, pNode);

    curNode->child = tempNode;

    for (int i = 1; i < argc; i++) {
        tempNode->next = va_arg(va_List, pNode);
        if (tempNode->next != NULL) {
            tempNode = tempNode->next;
        }
    }
    
    va_end(va_List);

    return curNode;
}

static inline char* newString(char*str){
    
    int len=strlen(str) + 1;
    char *nstr=(char*) malloc(sizeof(char)*(len));
    strncpy(nstr,str,len);
    return nstr;
}


static inline pNode newTokenNode(int lineNo, NodeType type, char* tokenName,
                                 char* tokenText) {
    pNode tokenNode = (pNode)malloc(sizeof(Node));
    // int nameLength = strlen(tokenName) + 1;
    // int textLength = strlen(tokenText) + 1;

    assert(tokenNode != NULL);

    tokenNode->lineNo = lineNo;
    tokenNode->type = type;



    tokenNode->name = newString(tokenName);
    tokenNode->val = newString(tokenText);

    tokenNode->child = NULL;
    tokenNode->next = NULL;

    return tokenNode;
}

static inline void delNode(pNode* node) {
    if (node == NULL) return;
    pNode p = *node;
    while (p->child != NULL) {
        pNode temp = p->child;
        p->child = p->child->next;
        delNode(&temp);
    }
    free(p->name);
    free(p->val);
    free(p);
    p = NULL;
}

static inline void printTreeInfo(pNode curNode, int height) {
    if (curNode == NULL) {
        return;
    }

    for (int i = 0; i < height; i++) {
        printf("  ");
    }
    fprintf(yyout,"%s", curNode->name);
    if (curNode->type == NOT_A_TOKEN) {
        fprintf(yyout," (%d)", curNode->lineNo);
    } else if (curNode->type == TOKEN_TYPE || curNode->type == TOKEN_ID ||
               curNode->type == TOKEN_INT) {
        fprintf(yyout,": %s", curNode->val);
    } else if (curNode->type == TOKEN_FLOAT) {
        fprintf(yyout,": %lf", atof(curNode->val));
    }
    fprintf(yyout,"\n");
    printTreeInfo(curNode->child, height + 1);
    printTreeInfo(curNode->next, height);
}

#endif //  ASTNode











