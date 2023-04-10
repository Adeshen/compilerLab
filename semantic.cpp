// #include "semantic_symbol_struct.h"
#include "semantic.h"


#ifndef SEMANTIC_CPP
#define SEMANTIC_cpp
extern pTable table;


#ifndef GLOBAL_FUNCTION
#define GLOBAL_FUNCTION
void traverseTree(pNode node){
    if (node == NULL) return;
    if (!strcmp(node->name, "ExtDef")) ExtDef(node);
    traverseTree(node->child);
    traverseTree(node->next);
}
#endif

#endif

#ifndef GENERATE_SYMBOL
#define GENERATE_SYMBOL

void ExtDef(pNode node) {

    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt
    assert(node != NULL);

    pType specifierType = Specifier(node->child);
    char* secondName = node->child->next->name;

    // Specifier SEMI  didn't need to solve
    
    if (!strcmp(secondName, "ExtDecList")) {
        // TODO: Specifier ExtDecList SEMI
        ExtDecList(node->child->next, specifierType);
    }

    else if (!strcmp(secondName, "FunDec")) {
        // TODO: Specifier FunDec CompSt
        FunDec(node->child->next, specifierType);
        CompSt(node->child->next->next, specifierType);
    }
    if (specifierType) deleteType(specifierType);
    
}

void ExtDecList(pNode node, pType specifier) {
    assert(node != NULL);
    // ExtDecList -> VarDec
    //             | VarDec COMMA ExtDecList
    pNode temp = node;
    while (temp) {
        pItem item = VarDec(temp->child, specifier);
        if (checkTableItemConflict(table, item)) {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->field->name);
            pError(REDEF_VAR, temp->lineNo, msg);
            deleteItem(item);
        } else {
            addTableItem(table, item);
        }
        if (temp->child->next) {
            temp = temp->next->next->child;
        } else {
            break;
        }
    }
}


pType Specifier(pNode node) {
    assert(node != NULL);
    // Specifier -> TYPE
    //            | StructSpecifier

    pNode t = node->child;
    // Specifier -> TYPE
    if (!strcmp(t->name, "TYPE")) {
        if (!strcmp(t->val, "float")) {
            return newType(BASIC, FLOAT_TYPE);
        } else {
            return newType(BASIC, INT_TYPE);
        }
    }
    // Specifier -> StructSpecifier
    else {
        return StructSpecifier(t);
    }
}

pType StructSpecifier(pNode node) {
    assert(node != NULL);
    // StructSpecifier -> STRUCT OptTag LC DefList RC
    //                  | STRUCT Tag

    // OptTag -> ID | e
    // Tag -> ID
    pType returnType = NULL;
    pNode t = node->child->next;
    // StructSpecifier->STRUCT OptTag LC DefList RC
    // printTreeInfo(t, 0);
    if (strcmp(t->name, "Tag")) {
        // addStructLayer(table);
        pItem structItem =
            newItem(table->stack->curStackDepth,
                    newFieldList("", newType(STRUCTURE, NULL, NULL)));
        if (!strcmp(t->name, "OptTag")) {
            setFieldListName(structItem->field, t->child->val);
            t = t->next;
        }
        // unnamed struct
        else {
            table->unNamedStructNum++;
            char structName[20] = {0};
            sprintf(structName, "%d", table->unNamedStructNum);
            // printf("unNamed struct's name is %s.\n", structName);
            setFieldListName(structItem->field, structName);
        }
        //现在我们进入结构体了！注意，报错信息会有不同！
        // addStackDepth(table->stack);
        if (!strcmp(t->next->name, "DefList")) {
            DefList(t->next, structItem);
        }

        if (checkTableItemConflict(table, structItem)) {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", structItem->field->name);
            pError(DUPLICATED_NAME, node->lineNo, msg);
            deleteItem(structItem);
        } else {
            returnType = newType(
                STRUCTURE, newString(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field));

            // printf("\nnew Type:\n");
            // printType(returnType);
            // printf("\n");

            if (!strcmp(node->child->next->name, "OptTag")) {
                addTableItem(table, structItem);
            }
            // OptTag -> e
            else {
                deleteItem(structItem);
            }
        }

        // minusStackDepth(table->stack);
        // minusStructLayer(table);
    }
    // StructSpecifier->STRUCT Tag
    else {
        pItem structItem = searchTableItem(table, t->child->val);
        if (structItem == NULL || !isStructDef(structItem)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", t->child->val);
            pError(UNDEF_STRUCT, node->lineNo, msg);
        } else
            returnType = newType(
                STRUCTURE, newString(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field));
    }
    // printType(returnType);
    return returnType;
}

pItem VarDec(pNode node, pType specifier) {
    assert(node != NULL);
    // VarDec -> ID
    //         | VarDec LB INT RB
    pNode id = node;
    // get ID
    while (id->child) id = id->child;
    pItem p = newItem(table->stack->curStackDepth, newFieldList(id->val, NULL));
    // return newItem(table->stack->curStackDepth,
    //                newFieldList(id->val, generateVarDecType(node,
    //                specifier)));

    // VarDec -> ID
    // printTreeInfo(node, 0);
    if (!strcmp(node->child->name, "ID")) {
        // printf("copy type tp %s.\n", node->child->val);
        p->field->type = copyType(specifier);
    }
    // VarDec -> VarDec LB INT RB
    else {
        pNode varDec = node->child;
        pType temp = specifier;
        // printf("VarDec -> VarDec LB INT RB.\n");
        while (varDec->next) {
            // printTreeInfo(varDec, 0);
            // printf("number: %s\n", varDec->next->next->val);
            // printf("temp type: %d\n", temp->kind);
            p->field->type =
                newType(ARRAY, copyType(temp), atoi(varDec->next->next->val));
            // printf("newType. newType: elem type: %d, elem size: %d.\n",
            //        p->field->type->u.array.elem->kind,
            //        p->field->type->u.array.size);
            temp = p->field->type;
            varDec = varDec->child;
        }
    }
    // printf("-------test VarDec ------\n");
    // printType(specifier);
    // printFieldList(p->field);
    // printf("-------test End ------\n");
    return p;
}


void FunDec(pNode node, pType returnType) {
    assert(node != NULL);
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    pItem p =
        newItem(table->stack->curStackDepth,
                newFieldList(node->child->val,
                             newType(FUNCTION, 0, NULL, copyType(returnType))));

    // FunDec -> ID LP VarList RP
    if (!strcmp(node->child->next->next->name, "VarList")) {
        VarList(node->child->next->next, p);
    }

    // FunDec -> ID LP RP don't need process

    // check redefine
    if (checkTableItemConflict(table, p)) {
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", p->field->name);
        pError(REDEF_FUNC, node->lineNo, msg);
        deleteItem(p);
        p = NULL;
    } else {
        addTableItem(table, p);
    }
}

void VarList(pNode node, pItem func) {
    assert(node != NULL);
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    addStackDepth(table->stack);
    int argc = 0;
    pNode temp = node->child;
    pFieldList cur = NULL;

    // VarList -> ParamDec
    pFieldList paramDec = ParamDec(temp);
    func->field->type->u.function.argv = copyFieldList(paramDec);
    cur = func->field->type->u.function.argv;
    argc++;

    // VarList -> ParamDec COMMA VarList
    while (temp->next) {
        temp = temp->next->next->child;
        paramDec = ParamDec(temp);
        if (paramDec) {
            cur->tail = copyFieldList(paramDec);
            cur = cur->tail;
            argc++;
        }
    }

    func->field->type->u.function.argc = argc;

    minusStackDepth(table->stack);
}

pFieldList ParamDec(pNode node) {
    assert(node != NULL);
    // ParamDec -> Specifier VarDec
    pType specifierType = Specifier(node->child);
    pItem p = VarDec(node->child->next, specifierType);
    if (specifierType) deleteType(specifierType);
    if (checkTableItemConflict(table, p)) {
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", p->field->name);
        pError(REDEF_VAR, node->lineNo, msg);
        deleteItem(p);
        return NULL;
    } else {
        addTableItem(table, p);
        return p->field;
    }
}

void CompSt(pNode node, pType returnType) {
    assert(node != NULL);
    // CompSt -> LC DefList StmtList RC
    // printTreeInfo(node, 0);
    addStackDepth(table->stack);
    pNode temp = node->child->next;
    if (!strcmp(temp->name, "DefList")) {
        DefList(temp, NULL);
        temp = temp->next;
    }
    if (!strcmp(temp->name, "StmtList")) {
        StmtList(temp, returnType);
    }
    //这是唯一一个地方会清空当前深度的
    clearCurDepthStackList(table);
}

void StmtList(pNode node, pType returnType) {
    // assert(node != NULL);
    // StmtList -> Stmt StmtList
    //           | e
    // printTreeInfo(node, 0);
    while (node) {
        Stmt(node->child, returnType);
        node = node->child->next;
    }
}



void Stmt(pNode node, pType returnType) {
    assert(node != NULL);
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt
    // printTreeInfo(node, 0);

    pType expType = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->child->name, "Exp")) expType = Exp(node->child);

    // Stmt -> CompSt
    else if (!strcmp(node->child->name, "CompSt"))
        CompSt(node->child, returnType);

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child->name, "RETURN")) {
        expType = Exp(node->child->next);

        // check return type
        if (!checkType(returnType, expType))
            pError(TYPE_MISMATCH_RETURN, node->lineNo,
                   "Type mismatched for return.");
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child->name, "IF")) {
        pNode stmt = node->child->next->next->next->next;
        expType = Exp(node->child->next->next);
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (stmt->next != NULL) Stmt(stmt->next->next, returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child->name, "WHILE")) {
        expType = Exp(node->child->next->next);
        Stmt(node->child->next->next->next->next, returnType);
    }

    if (expType) deleteType(expType);
}


void DefList(pNode node, pItem structInfo) {
    // assert(node != NULL);
    // DefList -> Def DefList
    //          | e
    while (node) {
        Def(node->child, structInfo);
        node = node->child->next;
    }
}

void Def(pNode node, pItem structInfo) {
    assert(node != NULL);
    // Def -> Specifier DecList SEMI
    // TODO:调用接口
    pType dectype = Specifier(node->child);
    //你总会得到一个正确的type
    DecList(node->child->next, dectype, structInfo);
    if (dectype) deleteType(dectype);
}

void DecList(pNode node, pType specifier, pItem structInfo) {
    assert(node != NULL);
    // DecList -> Dec
    //          | Dec COMMA DecList
    pNode temp = node;
    while (temp) {
        Dec(temp->child, specifier, structInfo);
        if (temp->child->next)
            temp = temp->child->next->next;
        else
            break;
    }
}


void Dec(pNode node, pType specifier, pItem structInfo) {
    assert(node != NULL);
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child->next == NULL) {
        if (structInfo != NULL) {
            // 结构体内，将VarDec返回的Item中的filedList
            // Copy判断是否重定义，无错则到结构体链表尾 记得delete掉Item
            pItem decitem = VarDec(node->child, specifier);
            pFieldList payload = decitem->field;
            pFieldList structField = structInfo->field->type->u.structure.field;
            pFieldList last = NULL;
            while (structField != NULL) {
                // then we have to check
                if (!strcmp(payload->name, structField->name)) {
                    //出现重定义，报错
                    char msg[100] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            decitem->field->name);
                    pError(REDEF_FEILD, node->lineNo, msg);
                    deleteItem(decitem);
                    return;
                } else {
                    last = structField;
                    structField = structField->tail;
                }
            }
            //新建一个fieldlist,删除之前的item
            if (last == NULL) {
                // that is good
                structInfo->field->type->u.structure.field =
                    copyFieldList(decitem->field);
            } else {
                last->tail = copyFieldList(decitem->field);
            }
            deleteItem(decitem);
        } else {
            // 非结构体内，判断返回的item有无冲突，无冲突放入表中，有冲突报错delete
            pItem decitem = VarDec(node->child, specifier);
            if (checkTableItemConflict(table, decitem)) {
                //出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                pError(REDEF_VAR, node->lineNo, msg);
                deleteItem(decitem);
            } else {
                addTableItem(table, decitem);
            }
        }
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        if (structInfo != NULL) {
            // 结构体内不能赋值，报错
            pError(REDEF_FEILD, node->lineNo,
                   "Illegal initialize variable in struct.");
        } else {
            // 判断赋值类型是否相符
            //如果成功，注册该符号
            pItem decitem = VarDec(node->child, specifier);
            pType exptype = Exp(node->child->next->next);
            if (checkTableItemConflict(table, decitem)) {
                //出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                pError(REDEF_VAR, node->lineNo, msg);
                deleteItem(decitem);
            }
            if (!checkType(decitem->field->type, exptype)) {
                //类型不相符
                //报错
                pError(TYPE_MISMATCH_ASSIGN, node->lineNo,
                       "Type mismatchedfor assignment.");
                deleteItem(decitem);
            }
            if (decitem->field->type && decitem->field->type->kind == ARRAY) {
                //报错，对非basic类型赋值
                pError(TYPE_MISMATCH_ASSIGN, node->lineNo,
                       "Illegal initialize variable.");
                deleteItem(decitem);
            } else {
                addTableItem(table, decitem);
            }
            // exp不出意外应该返回一个无用的type，删除
            if (exptype) deleteType(exptype);
        }
    }
}


pType Exp(pNode node) {
    assert(node != NULL);
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp
    //      | LP Exp RP
    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT
    pNode t = node->child;
    // exp will only check if the cal is right
    //  printTable(table);
    //二值运算
    if (!strcmp(t->name, "Exp")) {
        // 基本数学运算符
        if (strcmp(t->next->name, "LB") && strcmp(t->next->name, "DOT")) {
            pType p1 = Exp(t);
            pType p2 = Exp(t->next->next);
            pType returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(t->next->name, "ASSIGNOP")) {
                //检查左值
                pNode tchild = t->child;

                if (!strcmp(tchild->name, "FLOAT") ||
                    !strcmp(tchild->name, "INT")) {
                    //报错，左值
                    pError(LEFT_VAR_ASSIGN, t->lineNo,
                           "The left-hand side of an assignment must be "
                           "avariable.");

                } else if (!strcmp(tchild->name, "ID") ||
                           !strcmp(tchild->next->name, "LB") ||
                           !strcmp(tchild->next->name, "DOT")) {
                    if (!checkType(p1, p2)) {
                        //报错，类型不匹配
                        pError(TYPE_MISMATCH_ASSIGN, t->lineNo,
                               "Type mismatched for assignment.");
                    } else
                        returnType = copyType(p1);
                } else {
                    //报错，左值
                    pError(LEFT_VAR_ASSIGN, t->lineNo,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                }

            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY)) {
                    //报错，数组，结构体运算
                    pError(TYPE_MISMATCH_OP, t->lineNo,
                           "Type mismatched for operands.");
                } else if (!checkType(p1, p2)) {
                    //报错，类型不匹配
                    pError(TYPE_MISMATCH_OP, t->lineNo,
                           "Type mismatched for operands.");
                } else {
                    if (p1 && p2) {
                        returnType = copyType(p1);
                    }
                }
            }

            if (p1) deleteType(p1);
            if (p2) deleteType(p2);
            return returnType;
        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(t->next->name, "LB")) {
                //数组
                pType p1 = Exp(t);
                pType p2 = Exp(t->next->next);
                pType returnType = NULL;

                if (!p1) {
                    // 第一个exp为null，上层报错，这里不用再管
                } else if (p1 && p1->kind != ARRAY) {
                    //报错，非数组使用[]运算符
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->child->val);
                    pError(NOT_A_ARRAY, t->lineNo, msg);
                } else if (!p2 || p2->kind != BASIC ||
                           p2->u.basic != INT_TYPE) {
                    //报错，不用int索引[]
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",
                            t->next->next->child->val);
                    pError(NOT_A_INT, t->lineNo, msg);
                } else {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1) deleteType(p1);
                if (p2) deleteType(p2);
                return returnType;
            }
            // Exp -> Exp DOT ID
            else {
                pType p1 = Exp(t);
                pType returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE ||
                    !p1->u.structure.structName) {
                    //报错，对非结构体使用.运算符
                    pError(ILLEGAL_USE_DOT, t->lineNo, "Illegal use of \".\".");
                    // if (p1) deleteType(p1);
                } else {
                    pNode ref_id = t->next->next;
                    pFieldList structfield = p1->u.structure.field;
                    while (structfield != NULL) {
                        if (!strcmp(structfield->name, ref_id->val)) {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL) {
                        //报错，没有可以匹配的域名
                        // printf("Error type %d at Line %d: %s.\n", 14, t->lineNo,
                        //        "Non-ex");
                        char*msg=(char*)malloc(sizeof(char)*64);
                        sprintf(msg,"Non-existent field \"%s\".",ref_id->val);
                        pError(NONEXISTFIELD,t->lineNo,msg);
                        ;
                    } else {
                        returnType = copyType(structfield->type);
                    }
                }
                if (p1) deleteType(p1);
                return returnType;
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    //      | NOT Exp
    else if (!strcmp(t->name, "MINUS") || !strcmp(t->name, "NOT")) {
        pType p1 = Exp(t->next);
        pType returnType = NULL;
        if (!p1 || p1->kind != BASIC) {
            //报错，数组，结构体运算
            printf("Error type %d at Line %d: %s.\n", 7, t->lineNo,
                   "TYPE_MISMATCH_OP");
        } else {
            returnType = copyType(p1);
        }
        if (p1) deleteType(p1);
        return returnType;
    } else if (!strcmp(t->name, "LP")) {
        return Exp(t->next);
    }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(t->name, "ID") && t->next) {
        pItem funcInfo = searchTableItem(table, t->val);

        // function not find
        if (funcInfo == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->val);
            pError(UNDEF_FUNC, node->lineNo, msg);
            return NULL;
        } else if (funcInfo->field->type->kind != FUNCTION) {
            char msg[100] = {0};
            sprintf(msg, "\"i\" is not a function.", t->val);
            pError(NOT_A_FUNC, node->lineNo, msg);
            return NULL;
        }
        // Exp -> ID LP Args RP
        else if (!strcmp(t->next->next->name, "Args")) {
            Args(t->next->next, funcInfo);
            return copyType(funcInfo->field->type->u.function.returnType);
        }
        // Exp -> ID LP RP
        else {
            if (funcInfo->field->type->u.function.argc != 0) {
                char msg[100] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        funcInfo->field->name,
                        funcInfo->field->type->u.function.argc);
                pError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
            }
            return copyType(funcInfo->field->type->u.function.returnType);
        }
    }
    // Exp -> ID
    else if (!strcmp(t->name, "ID")) {
        pItem tp = searchTableItem(table, t->val);
        if (tp == NULL || isStructDef(tp)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->val);
            pError(UNDEF_VAR, t->lineNo, msg);
            return NULL;
        } else {
            // good
            return copyType(tp->field->type);
        }
    } else {
        // Exp -> FLOAT
        if (!strcmp(t->name, "FLOAT")) {
            return newType(BASIC, FLOAT_TYPE);
        }
        // Exp -> INT
        else {
            return newType(BASIC, INT_TYPE);
        }
    }
}

void Args(pNode node, pItem funcInfo) {
    assert(node != NULL);
    // Args -> Exp COMMA Args
    //       | Exp
    // printTreeInfo(node, 0);
    pNode temp = node;
    pFieldList arg = funcInfo->field->type->u.function.argv;
    // printf("-----function atgs-------\n");
    // printFieldList(arg);
    // printf("---------end-------------\n");


    while (temp) {
        if (arg == NULL) {
            char msg[100] = {0};
            char msgs[32]={0};
            strcat(msgs,funcInfo->field->name);
            getTypeStr(funcInfo->field->type,msgs);
            sprintf(
                msg, "too many arguments to function \"%s\", except %d args.",
                msgs, funcInfo->field->type->u.function.argc);
            pError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
            break;
        }
        pType realType = Exp(temp->child);
        // printf("=======arg type=========\n");
        // printType(realType);
        // printf("===========end==========\n");
        if (!checkType(realType, arg->type)) {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funcInfo->field->name);
            pError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
            if (realType) deleteType(realType);
            return;
        }
        if (realType) deleteType(realType);

        arg = arg->tail;
        if (temp->child->next) {
            temp = temp->child->next->next;
        } else {
            break;
        }
    }
    if (arg != NULL) {
        char msg[100] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
        pError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
    }
}



#endif



