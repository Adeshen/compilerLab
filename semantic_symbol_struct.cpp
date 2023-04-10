/*
this is the struct method implement of semantic.h


*/


#include "semantic_symbol_struct.h"

#ifndef TYPE_FUNCTION
#define TYPE_FUNCTION
bool isStructDef(pItem src) {
    if (src == NULL) return false;
    if (src->field->type->kind != STRUCTURE) return false;
    if (src->field->type->u.structure.structName) return false;
    return true;
}


bool checkType(pType type1, pType type2) {
    if (type1 == NULL || type2 == NULL) return true;
    if (type1->kind == FUNCTION || type2->kind == FUNCTION) return false;
    if (type1->kind != type2->kind)
        return false;
    else {
        assert(type1->kind == BASIC || type1->kind == ARRAY ||
               type1->kind == STRUCTURE);
        switch (type1->kind) {
            case BASIC:
                return type1->u.basic == type2->u.basic;
            case ARRAY:
                return checkType(type1->u.array.elem, type2->u.array.elem);
            case STRUCTURE:
                return !strcmp(type1->u.structure.structName,
                               type2->u.structure.structName);
        }
    }
}

void printType(pType type) {
    if (type == NULL) {
        printf("type is NULL.\n");
    } else {
        printf("type kind: %d\n", type->kind);
        switch (type->kind) {
            case BASIC:
                printf("type basic: %d\n", type->u.basic);
                break;
            case ARRAY:
                printf("array size: %d\n", type->u.array.size);
                printType(type->u.array.elem);
                break;
            case STRUCTURE:
                if (!type->u.structure.structName)
                    printf("struct name is NULL\n");
                else {
                    printf("struct name is %s\n", type->u.structure.structName);
                }
                printFieldList(type->u.structure.field);
                break;
            case FUNCTION:
                printf("function argc is %d\n", type->u.function.argc);
                printf("function args:\n");
                printFieldList(type->u.function.argv);
                printf("function return type:\n");
                printType(type->u.function.returnType);
                break;
        }
    }
}

// Type functions
pType newType(Kind kind, ...) {
    pType p = (pType)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = kind;
    va_list vaList;


    assert(kind == BASIC || kind == ARRAY || kind == STRUCTURE || kind == FUNCTION);
    switch (kind) {
        case BASIC:
            va_start(vaList, 1);
            p->u.basic =(BasicType)va_arg(vaList,int );
            break;
        case ARRAY:
            va_start(vaList, 2);
            p->u.array.elem = va_arg(vaList, pType);
            p->u.array.size = va_arg(vaList, int);
            break;
        case STRUCTURE:
            va_start(vaList, 2);
            p->u.structure.structName = va_arg(vaList, char*);
            p->u.structure.field = va_arg(vaList, pFieldList);
            break;
        case FUNCTION:
            va_start(vaList, 3);
            p->u.function.argc = va_arg(vaList, int);
            p->u.function.argv = va_arg(vaList, pFieldList);
            p->u.function.returnType = va_arg(vaList, pType);
            break;
    }
    va_end(vaList);
    return p;
}


pType copyType(pType src) {
    if (src == NULL) return NULL;
    pType p = (pType)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = src->kind;
    assert(p->kind == BASIC || p->kind == ARRAY || p->kind == STRUCTURE ||
           p->kind == FUNCTION);
    switch (p->kind) {
        case BASIC:
            p->u.basic = src->u.basic;
            break;
        case ARRAY:
            p->u.array.elem = copyType(src->u.array.elem);
            p->u.array.size = src->u.array.size;
            break;
        case STRUCTURE:
            p->u.structure.structName = newString(src->u.structure.structName);
            p->u.structure.field = copyFieldList(src->u.structure.field);
            break;
        case FUNCTION:
            p->u.function.argc = src->u.function.argc;
            p->u.function.argv = copyFieldList(src->u.function.argv);
            p->u.function.returnType = copyType(src->u.function.returnType);
            break;
    }

    return p;
}

void deleteType(pType type) {
    assert(type != NULL);
    assert(type->kind == BASIC || type->kind == ARRAY ||
           type->kind == STRUCTURE || type->kind == FUNCTION);
pFieldList temp = NULL;
    // pFieldList tDelete = NULL;
    switch (type->kind) {
        case BASIC:
            break;
        case ARRAY:
            deleteType(type->u.array.elem);
            type->u.array.elem = NULL;
            break;
        case STRUCTURE:
            if (type->u.structure.structName)
                free(type->u.structure.structName);
            type->u.structure.structName = NULL;

            temp = type->u.structure.field;
            while (temp) {
                pFieldList tDelete = temp;
                temp = temp->tail;
                deleteFieldList(tDelete);
            }
            type->u.structure.field = NULL;
            break;
        case FUNCTION:
            deleteType(type->u.function.returnType);
            type->u.function.returnType = NULL;
            temp = type->u.function.argv;
            while (temp) {
                pFieldList tDelete = temp;
                temp = temp->tail;
                deleteFieldList(tDelete);
            }
            type->u.function.argv = NULL;
            break;
    }
    free(type);
}




#endif // !TYPE_FUNCTION

#ifndef FIELD_FUNCTION
#define FIELD_FUNCTION


pFieldList newFieldList(char* newName, pType newType) {
    pFieldList p = (pFieldList)malloc(sizeof(FieldList));
    assert(p != NULL);
    p->name = newString(newName);
    p->type = newType;
    p->tail = NULL;
    return p;
}



pFieldList copyFieldList(pFieldList src) {
    assert(src != NULL);
    pFieldList head = NULL, cur = NULL;
    pFieldList temp = src;

    while (temp) {
        if (!head) {
            head = newFieldList(temp->name, copyType(temp->type));
            cur = head;
            temp = temp->tail;
        } else {
            cur->tail = newFieldList(temp->name, copyType(temp->type));
            cur = cur->tail;
            temp = temp->tail;
        }
    }
    return head;
}

void deleteFieldList(pFieldList fieldList) {
    assert(fieldList != NULL);
    if (fieldList->name) {
        free(fieldList->name);
        fieldList->name = NULL;
    }
    if (fieldList->type) deleteType(fieldList->type);
    fieldList->type = NULL;
    free(fieldList);
}

void setFieldListName(pFieldList p, char* newName) {
    assert(p != NULL && newName != NULL);
    if (p->name != NULL) {
        free(p->name);
    }
    p->name = newString(newName);
}

void printFieldList(pFieldList fieldList) {
    if (fieldList == NULL)
        printf("fieldList is NULL\n");
    else {
        printf("fieldList name is: %s\n", fieldList->name);
        printf("FieldList Type:\n");
        printType(fieldList->type);
        printFieldList(fieldList->tail);
    }
}





#endif // !FIELD_FUNCTION

#ifndef ITEM_FUNCTION
#define ITEM_FUNCTION
pItem newItem(int symbolDepth, pFieldList pfield) {
    pItem p = (pItem)malloc(sizeof(TableItem));
    assert(p != NULL);
    p->symbolDepth = symbolDepth;
    p->field = pfield;
    p->nextHash = NULL;
    p->nextSymbol = NULL;
    return p;
}

void deleteItem(pItem item) {
    assert(item != NULL);
    if (item->field != NULL) deleteFieldList(item->field);
    free(item);
}

#endif // !ITEM_FUNCTION

#ifndef HASH_FUNCTION
#define HASH_FUNCTION

pHash newHash() {
    pHash p = (pHash)malloc(sizeof(HashTable));
    assert(p != NULL);
    p->hashArray = (pItem*)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->hashArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->hashArray[i] = NULL;
    }
    return p;
}

void deleteHash(pHash hash) {
    assert(hash != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pItem temp = hash->hashArray[i];
        while (temp) {
            pItem tdelete = temp;
            temp = temp->nextHash;
            deleteItem(tdelete);
        }
        hash->hashArray[i] = NULL;
    }
    free(hash->hashArray);
    hash->hashArray = NULL;
    free(hash);
}

pItem getHashHead(pHash hash, int index) {
    assert(hash != NULL);
    return hash->hashArray[index];
}

void setHashHead(pHash hash, int index, pItem newVal) {
    assert(hash != NULL);
    hash->hashArray[index] = newVal;
}






#endif // !HASH_FUNCTION

#ifndef TABLE_FUNCTION
#define TABLE_FUNCTION

pTable initTable() {
    pTable table = (pTable)malloc(sizeof(Table));
    assert(table != NULL);
    table->hash = newHash();
    table->stack = newStack();
    table->unNamedStructNum = 0;
    return table;
};

void deleteTable(pTable table) {
    deleteHash(table->hash);
    table->hash = NULL;
    deleteStack(table->stack);
    table->stack = NULL;
    free(table);
};

pItem searchTableItem(pTable table, char* name) {
    unsigned hashCode = getHashCode(name);
    pItem temp = getHashHead(table->hash, hashCode);
    if (temp == NULL) return NULL;
    while (temp) {
        if (!strcmp(temp->field->name, name)) return temp;
        temp = temp->nextHash;
    }
    return NULL;
}

// Return false -> no confliction, true -> has confliction
bool checkTableItemConflict(pTable table, pItem item) {
    pItem temp = searchTableItem(table, item->field->name);
    if (temp == NULL) return false;
    while (temp) {
        if (!strcmp(temp->field->name, item->field->name)) {
            if (temp->field->type->kind == STRUCTURE ||
                item->field->type->kind == STRUCTURE)
                return true;
            if (temp->symbolDepth == table->stack->curStackDepth) return true;
        }
        temp = temp->nextHash;
    }
    return false;
}

void addTableItem(pTable table, pItem item) {
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    pHash hash = table->hash;
    pStack stack = table->stack;

    item->nextSymbol = getCurDepthStackHead(stack);
    setCurDepthStackHead(stack, item);

    item->nextHash = getHashHead(hash, hashCode);
    setHashHead(hash, hashCode, item);
}

void deleteTableItem(pTable table, pItem item) {
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    if (item == getHashHead(table->hash, hashCode))
        setHashHead(table->hash, hashCode, item->nextHash);
    else {
        pItem cur = getHashHead(table->hash, hashCode);
        pItem last = cur;
        while (cur != item) {
            last = cur;
            cur = cur->nextHash;
        }
        last->nextHash = cur->nextHash;
    }
    deleteItem(item);
}

void clearCurDepthStackList(pTable table) {
    assert(table != NULL);
    pStack stack = table->stack;
    pItem temp = getCurDepthStackHead(stack);
    while (temp) {
        pItem tDelete = temp;
        temp = temp->nextSymbol;
        deleteTableItem(table, tDelete);
    }
    setCurDepthStackHead(stack, NULL);
    minusStackDepth(stack);
}

// for Debug
void printTable(pTable table) {
    printf("----------------hash_table----------------\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pItem item = getHashHead(table->hash, i);
        if (item) {
            printf("[%d]", i);
            while (item) {
                printf(" -> name: %s depth: %d\n", item->field->name,
                       item->symbolDepth);
                printf("========FiledList========\n");
                printFieldList(item->field);
                printf("===========End===========\n");
                item = item->nextHash;
            }
            printf("\n");
        }
    }
    printf("-------------------end--------------------\n");
}


#endif // !TABLE_FUNCTION

#ifndef STACK_FUNCTION
#define STACK_FUNCTION


pStack newStack() {
    pStack p = (pStack)malloc(sizeof(Stack));
    assert(p != NULL);
    p->stackArray = (pItem*)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->stackArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->stackArray[i] = NULL;
    }
    p->curStackDepth = 0;
    return p;
}

void deleteStack(pStack stack){
    assert(stack != NULL);
    free(stack->stackArray);
    stack->stackArray = NULL;
    stack->curStackDepth = 0;
    free(stack);
}

void addStackDepth(pStack stack) {
    assert(stack != NULL);
    stack->curStackDepth++;
}

void minusStackDepth(pStack stack) {
    assert(stack != NULL);
    stack->curStackDepth--;
}

pItem getCurDepthStackHead(pStack stack) {
    assert(stack != NULL);
    return stack->stackArray[stack->curStackDepth];
    // return p == NULL ? NULL : p->stackArray[p->curStackDepth];
}

void setCurDepthStackHead(pStack stack, pItem newVal) {
    assert(stack != NULL);
    stack->stackArray[stack->curStackDepth] = newVal;
}

#endif // !STACK_FUNCTION