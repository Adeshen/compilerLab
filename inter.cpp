#include "inter.h"
#include <string.h>

// extern pInterCodes interCodeList;

void genInterCodes(pNode node) {
    if (node == NULL) return;
    if (!strcmp(node->name, "ExtDefList"))
        translateExtDefList(node);
    else {
        genInterCodes(node->child);
        genInterCodes(node->next);
    }
}


void genInterCode(int kind, ...) {
    va_list vaList;
    pOperand temp = NULL;
    pOperand result = NULL, op1 = NULL, op2 = NULL, relop = NULL;
    int size = 0;
    pInterCodes newCode = NULL;
    assert(kind >= 0 && kind < 19);
    switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(vaList, 1);
            op1 = va_arg(vaList, pOperand);
            // if (op1->kind == OP_ADDRESS) {
            //     temp = newTemp();
            //     genInterCode(IR_READ_ADDR, temp, op1);
            //     op1 = temp;
            // }
            newCode = newInterCodes(newInterCode(kind, op1));
            addInterCode(interCodeList, newCode);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            va_start(vaList, 2);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            if (kind == IR_ASSIGN &&
                (op1->kind == OP_ADDRESS || op2->kind == OP_ADDRESS)) {
                if (op1->kind == OP_ADDRESS && op2->kind != OP_ADDRESS)
                    genInterCode(IR_WRITE_ADDR, op1, op2);
                else if (op2->kind == OP_ADDRESS && op1->kind != OP_ADDRESS)
                    genInterCode(IR_READ_ADDR, op1, op2);
                else {
                    temp = newTemp();
                    genInterCode(IR_READ_ADDR, temp, op2);
                    genInterCode(IR_WRITE_ADDR, op1, temp);
                }
            } else {
                newCode = newInterCodes(newInterCode(kind, op1, op2));
                addInterCode(interCodeList, newCode);
            }
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            va_start(vaList, 3);
            result = va_arg(vaList, pOperand);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            if (op1->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op1);
                op1 = temp;
            }
            if (op2->kind == OP_ADDRESS) {
                temp = newTemp();
                genInterCode(IR_READ_ADDR, temp, op2);
                op2 = temp;
            }
            newCode = newInterCodes(newInterCode(kind, result, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
        case IR_DEC:
            // TODO:
            va_start(vaList, 2);
            op1 = va_arg(vaList, pOperand);
            size = va_arg(vaList, int);
            newCode = newInterCodes(newInterCode(kind, op1, size));
            addInterCode(interCodeList, newCode);
            break;
        case IR_IF_GOTO:
            // TODO:
            va_start(vaList, 4);
            result = va_arg(vaList, pOperand);
            relop = va_arg(vaList, pOperand);
            op1 = va_arg(vaList, pOperand);
            op2 = va_arg(vaList, pOperand);
            newCode =
                newInterCodes(newInterCode(kind, result, relop, op1, op2));
            addInterCode(interCodeList, newCode);
            break;
    }
}

void translateExtDefList(pNode node) {
    // ExtDefList -> ExtDef ExtDefList
    //             | e
    while (node) {
        translateExtDef(node->child);
        node = node->child->next;
    }
}



void translateExtDef(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt
    //         | Specifier FunDec TSEMICOLON 
       
    // 因为没有全局变量使用，
    // ExtDecList不涉及中间代码生成，类型声明也不涉及，所以只需要处理FunDec和CompSt
    if (!strcmp(node->child->next->name, "FunDec")) {
        translateFunDec(node->child->next);
        if(!strcmp(node->child->next->next->name, "CompSt")){
            translateCompSt(node->child->next->next);
        }
    }
}


void translateFunDec(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    genInterCode(IR_FUNCTION,
                 newOperand(OP_FUNCTION, newString(node->child->val)));
    // pInterCodes func = newInterCodes(newInterCode(
    //     IR_FUNCTION, newOperand(OP_FUNCTION, newString(node->child->val))));
    // addInterCode(interCodeList, func);

    pItem funcItem = searchTableItem(table, node->child->val);
    pFieldList temp = funcItem->field->type->u.function.argv;
    while (temp) {
        genInterCode(IR_PARAM, newOperand(OP_VARIABLE, newString(temp->name)));
        // pInterCodes arg = newInterCodes(newInterCode(
        //     IR_PARAM, newOperand(OP_VARIABLE, newString(temp->name))));
        // addInterCode(interCodeList, arg);
        temp = temp->tail;
    }
}


void translateCompSt(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // CompSt -> LC DefList StmtList RC
    pNode temp = node->child->next;
    if (!strcmp(temp->name, "DefList")) {
        translateDefList(temp);
        temp = temp->next;
    }
    if (!strcmp(temp->name, "StmtList")) {
        translateStmtList(temp);
    }
}


void translateDefList(pNode node) {
    if (interError) return;
    // DefList -> Def DefList
    //          | e
    while (node) {
        translateDef(node->child);
        node = node->child->next;
    }
}

void translateDef(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // Def -> Specifier DecList SEMI
    translateDecList(node->child->next);
}

void translateDecList(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // DecList -> Dec
    //          | Dec COMMA DecList
    pNode temp = node;
    while (temp) {
        translateDec(temp->child);
        if (temp->child->next)
            temp = temp->child->next->next;
        else
            break;
    }
}


void translateDec(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child->next == NULL) {
        translateVarDec(node->child, NULL);
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        pOperand t1 = newTemp();
        translateVarDec(node->child, t1);
        pOperand t2 = newTemp();
        translateExp(node->child->next->next, t2);
        genInterCode(IR_ASSIGN, t1, t2);
    }
}

void translateVarDec(pNode node, pOperand place) {
    assert(node != NULL);
    if (interError) return;
    // VarDec -> ID
    //         | VarDec LB INT RB
    // return ;
    if (!strcmp(node->child->name, "ID")) {
        pItem temp = searchTableItem(table, node->child->val);
        
        pType type = temp->field->type;
        if (type->kind == BASIC) {
            if (place) {
                interCodeList->tempVarNum--;
                setOperand(place, OP_VARIABLE,
                           (void*)newString(temp->field->name));
            }
        } else if (type->kind == ARRAY) {
        
            if (type->u.array.elem->kind == ARRAY) {
                // interError = true;
                // printf(
                //     "Cannot translate: Code containsvariables of "
                //     "multi-dimensional array type or parameters of array when declaring"
                //     "type.\n");
                // return;
                genInterCode(
                    IR_DEC,
                    newOperand(OP_VARIABLE,newString(temp->field->name)),
                    getSize(type)
                );

            } else {
                genInterCode(
                    IR_DEC,
                    newOperand(OP_VARIABLE, newString(temp->field->name)),
                    getSize(type));
            }
        } else if (type->kind == STRUCTURE) {
            // 3.1选做
            genInterCode(IR_DEC,
                         newOperand(OP_VARIABLE, newString(temp->field->name)),
                         getSize(type));
        }
    } else {
        translateVarDec(node->child, place);
    }
}


void translateStmtList(pNode node) {
    if (interError) return;
    // StmtList -> Stmt StmtList
    //           | e
    while (node) {
        translateStmt(node->child);
        node = node->child->next;
    }
}

void translateStmt(pNode node) {
    assert(node != NULL);
    if (interError) return;
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt

    // Stmt -> Exp SEMI

    if (!strcmp(node->child->name, "Exp")) {
        translateExp(node->child, NULL);
    }

    // Stmt -> CompSt
    else if (!strcmp(node->child->name, "CompSt")) {
        translateCompSt(node->child);
    }

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child->name, "RETURN")) {
        pOperand t1 = newTemp();
        translateExp(node->child->next, t1);
        genInterCode(IR_RETURN, t1);
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child->name, "IF")) {
        pNode exp = node->child->next->next;
        pNode stmt = exp->next->next;
        pOperand label1 = newLabel();
        pOperand label2 = newLabel();

        translateCond(exp, label1, label2);
        genInterCode(IR_LABEL, label1);
        translateStmt(stmt);
        if (stmt->next == NULL) {
            genInterCode(IR_LABEL, label2);
        }
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        else {
            pOperand label3 = newLabel();
            genInterCode(IR_GOTO, label3);
            genInterCode(IR_LABEL, label2);
            translateStmt(stmt->next->next);
            genInterCode(IR_LABEL, label3);
        }

    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child->name, "WHILE")) {
        pOperand label1 = newLabel();
        pOperand label2 = newLabel();
        pOperand label3 = newLabel();

        genInterCode(IR_LABEL, label1);
        translateCond(node->child->next->next, label2, label3);
        genInterCode(IR_LABEL, label2);
        translateStmt(node->child->next->next->next->next);
        genInterCode(IR_GOTO, label1);
        genInterCode(IR_LABEL, label3);
    }
}

void translateExp(pNode node, pOperand place) {
    assert(node != NULL);
    if (interError) return;
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp

    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT

    // Exp -> LP Exp RP
    if (!strcmp(node->child->name, "LP"))
        translateExp(node->child->next, place);

    else if (!strcmp(node->child->name, "Exp") ||
             !strcmp(node->child->name, "NOT")) {
        // 条件表达式 和 基本表达式
        if (strcmp(node->child->next->name, "LB") &&
            strcmp(node->child->next->name, "DOT")) {
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | NOT Exp
            if (!strcmp(node->child->next->name, "AND") ||
                !strcmp(node->child->next->name, "OR") ||
                !strcmp(node->child->next->name, "RELOP") ||
                !strcmp(node->child->name, "NOT")) {
                pOperand label1 = newLabel();
                pOperand label2 = newLabel();
                pOperand true_num = newOperand(OP_CONSTANT, 1);
                pOperand false_num = newOperand(OP_CONSTANT, 0);
                genInterCode(IR_ASSIGN, place, false_num);
                translateCond(node, label1, label2);
                genInterCode(IR_LABEL, label1);
                genInterCode(IR_ASSIGN, place, true_num);
            } else {
                // Exp -> Exp ASSIGNOP Exp
                if (!strcmp(node->child->next->name, "ASSIGNOP")) {
                    pOperand t2 = newTemp();
                    translateExp(node->child->next->next, t2);
                    pOperand t1 = newTemp();
                    translateExp(node->child, t1);
                    genInterCode(IR_ASSIGN, t1, t2);
                } else {
                    pOperand t1 = newTemp();
                    translateExp(node->child, t1);
                    pOperand t2 = newTemp();
                    translateExp(node->child->next->next, t2);
                    // Exp -> Exp PLUS Exp
                    if (!strcmp(node->child->next->name, "PLUS")) {
                        genInterCode(IR_ADD, place, t1, t2);
                    }
                    // Exp -> Exp MINUS Exp
                    else if (!strcmp(node->child->next->name, "MINUS")) {
                        genInterCode(IR_SUB, place, t1, t2);
                    }
                    // Exp -> Exp STAR Exp
                    else if (!strcmp(node->child->next->name, "STAR")) {
                        genInterCode(IR_MUL, place, t1, t2);
                    }
                    // Exp -> Exp DIV Exp
                    else if (!strcmp(node->child->next->name, "DIV")) {
                        genInterCode(IR_DIV, place, t1, t2);
                    }
                }
            }

        }
        // 数组和结构体访问
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(node->child->next->name, "LB")) {
                //数组
                if (node->child->child->next &&
                    !strcmp(node->child->child->next->name, "LB") ) {
                    //多维数组，报错
                    // interError = true;
                    // printf(
                    //     "Cannot translate: Code containsvariables of "
                    //     "multi-dimensional array type or parameters of array "
                    //     "type when used in expression.\n");
                    // return;


                    //1.得知偏移数据,源于每一层循环
                    //2.直接使用循环, 找到当中


                    pNode curNode=node;   
                    pOperand idxs[100];
                    int top=0;
                    
                    //  循环找到 数组名字，并在各层 中找到 [idx],并存储在idxs数组中
                    //   例如arr[id0][id1][id2]
                    while(strcmp(curNode->child->name,"ID")){
                       
                        idxs[top]=newTemp();
                        translateExp(curNode->child->next->next, idxs[top]);
                        curNode=curNode->child;
                        ++top;
                    }

                    pItem item = searchTableItem(table, curNode->child->val);
                    pOperand base = newTemp();
                    translateExp(curNode, base);

                    pType curType = item->field->type;
                    // pOperand offset = newTemp();
                    pOperand low_offset = newTemp();
                    pOperand width;
                    pOperand target;
                    //根据名字循环计算，每一层的偏移量，当层的偏移量存储在low_offset, 最终总的偏移存储在offset中。
                    
                    if (base->kind == OP_VARIABLE) {
                        // printf("非结构体数组访问\n");
                        // target = newTemp();
                        // genInterCode(IR_GET_ADDR, target, base);
                        genInterCode(IR_GET_ADDR, place,base);
                    } else {
                        // printf("结构体数组访问\n");
                        genInterCode(IR_ASSIGN, place,base);
                    }   
                    
                    while(top>0){
                        top--;
                        width = newOperand(
                            OP_CONSTANT, getSize(curType->u.array.elem));
                        genInterCode(IR_MUL, low_offset, idxs[top], width);
                        genInterCode(IR_ADD, place, place, low_offset);
                        curType=curType->u.array.elem;
                    }
                 
                    //将基地址和偏移量相加得到最终地址
                    // genInterCode(IR_ADD, place, target, offset);
                    place->kind = OP_ADDRESS;
                    interCodeList->lastArrayName = base->u.name;
                } else {
                    pOperand idx = newTemp();
                    translateExp(node->child->next->next, idx);
                    pOperand base = newTemp();
                    translateExp(node->child, base);

                    pOperand width;
                    pOperand offset = newTemp();
                    pOperand target;
                    // 根据假设，Exp1只会展开为 Exp DOT ID 或 ID
                    // 我们让前一种情况吧ID作为name回填进place返回到这里的base处，在语义分析时将结构体变量也填进表（因为假设无重名），这样两种情况都可以查表得到。
                    pItem item = searchTableItem(table, base->u.name);
                    assert(item->field->type->kind == ARRAY);
                    width = newOperand(
                        OP_CONSTANT, getSize(item->field->type->u.array.elem));
                    genInterCode(IR_MUL, offset, idx, width);
                    // 如果是ID[Exp],
                    // 则需要对ID取址，如果前面是结构体内访问，则会返回一个地址类型，不需要再取址
                    // if (base->kind == OP_VARIABLE) {
                    //     // printf("非结构体数组访问\n");
                    //     target = newTemp();
                    //     genInterCode(IR_GET_ADDR, target, base);
                        
                    // } else {
                    //     // printf("结构体数组访问\n");
                    //     target = base;
                    // }
                    target = base;
                    genInterCode(IR_ADD, place, target, offset);
                    place->kind = OP_ADDRESS;
                    interCodeList->lastArrayName = base->u.name;
                }


            }
            // Exp -> Exp DOT ID
            else {
                //结构体
                pOperand temp = newTemp();
                translateExp(node->child, temp);
                // 两种情况，Exp直接为一个变量，则需要先取址，若Exp为数组或者多层结构体访问或结构体形参，则target会被填成地址，可以直接用。
                pOperand target;

                // if (temp->kind == OP_ADDRESS) {
                //     target = newOperand(temp->kind, temp->u.name);
                //     // target->isAddr = TRUE;
                    
                // } else {
                //     target = newTemp();  
                //     genInterCode(IR_GET_ADDR, target, temp);
                // }
                target = newOperand(temp->kind, temp->u.name);
                pOperand id = newOperand(
                    OP_VARIABLE, newString(node->child->next->next->val));
                int offset = 0;
                pItem item = searchTableItem(table, temp->u.name);
                //结构体数组，temp是临时变量，查不到表，需要用处理数组时候记录下的数组名老查表
                if (item == NULL) {
                    item = searchTableItem(table, interCodeList->lastArrayName);
                }

                pFieldList tmp;
                // 结构体数组 eg: a[5].b
                if (item->field->type->kind == ARRAY) {
                    tmp = item->field->type->u.array.elem->u.structure.field;
                }
                // 一般结构体
                else {
                    tmp = item->field->type->u.structure.field;
                }
                // 遍历获得offset
                while (tmp) {
                    if (!strcmp(tmp->name, id->u.name)) break;
                    offset += getSize(tmp->type);
                    tmp = tmp->tail;
                }

                pOperand tOffset = newOperand(OP_CONSTANT, offset);
                if (place) {
                    genInterCode(IR_ADD, place, target, tOffset);
                    // 为了处理结构体里的数组把id名通过place回传给上层


                    setOperand(place, OP_ADDRESS, (void*)newString(id->u.name));
                    // place->isAddr = TRUE;
                }
            }
        }
    }
    //单目运算符
    // Exp -> MINUS Exp
    else if (!strcmp(node->child->name, "MINUS")) {
        pOperand t1 = newTemp();
        translateExp(node->child->next, t1);
        pOperand zero = newOperand(OP_CONSTANT, 0);
        genInterCode(IR_SUB, place, zero, t1);
    }
    // // Exp -> NOT Exp
    // else if (!strcmp(node->child->name, "NOT")) {
    //     pOperand label1 = newLabel();
    //     pOperand label2 = newLabel();
    //     pOperand true_num = newOperand(OP_CONSTANT, 1);
    //     pOperand false_num = newOperand(OP_CONSTANT, 0);
    //     genInterCode(IR_ASSIGN, place, false_num);
    //     translateCond(node, label1, label2);
    //     genInterCode(IR_LABEL, label1);
    //     genInterCode(IR_ASSIGN, place, true_num);
    // }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(node->child->name, "ID") && node->child->next) {
        pOperand funcTemp =
            newOperand(OP_FUNCTION, newString(node->child->val));
        // Exp -> ID LP Args RP
        if (!strcmp(node->child->next->next->name, "Args")) {
            pArgList argList = newArgList();
            translateArgs(node->child->next->next, argList);
            if (!strcmp(node->child->val, "write")) {
                genInterCode(IR_WRITE, argList->head->op);
            } else {
                pArg argTemp = argList->head;
                while (argTemp) {
                    if (argTemp->op->kind == OP_VARIABLE) {
                        pItem item =
                            searchTableItem(table, argTemp->op->u.name);

                        // 结构体作为参数需要传址
                        if (item && (item->field->type->kind == STRUCTURE||item->field->type->kind == ARRAY) ) {
                            pOperand varTemp = newTemp();
                            genInterCode(IR_GET_ADDR, varTemp, argTemp->op);
                            pOperand varTempCopy =
                                newOperand(OP_ADDRESS, varTemp->u.name);
                        
                            genInterCode(IR_ARG, varTempCopy);
                        }else{
                            genInterCode(IR_ARG, argTemp->op);
                        }
                        
                    }
                    // 一般参数直接传值
                    else {
                        genInterCode(IR_ARG, argTemp->op);
                    }
                    argTemp = argTemp->next;
                }
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    pOperand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
        // Exp -> ID LP RP
        else {
            if (!strcmp(node->child->val, "read")) {
                genInterCode(IR_READ, place);
            } else {
                if (place) {
                    genInterCode(IR_CALL, place, funcTemp);
                } else {
                    pOperand temp = newTemp();
                    genInterCode(IR_CALL, temp, funcTemp);
                }
            }
        }
    }
    // Exp -> ID
    else if (!strcmp(node->child->name, "ID")) {
        pItem item = searchTableItem(table, node->child->val);
        // 根据讲义，因为结构体不允许赋值，结构体做形参时是传址的方式
        interCodeList->tempVarNum--;
        // if (item->field->isArg ) {
        //     setOperand(place, OP_ADDRESS, (void*)newString(node->child->val));
        //     // place->isAddr = TRUE;
            
        // }
        // // 非结构体参数情况都当做变量处理
        // else {
        //     setOperand(place, OP_VARIABLE, (void*)newString(node->child->val));
        // }
        setOperand(place, OP_VARIABLE, (void*)newString(node->child->val));
        // pOperand t1 = newOperand(OP_VARIABLE, id_name->field->name);
        // genInterCode(IR_ASSIGN, place, t1);
    } else {
        // // Exp -> FLOAT
        // 无浮点数常数
        // if (!strcmp(node->child->name, "FLOAT")) {
        //     pOperand t1 = newOperand(OP_CONSTANT, node->child->val);
        //     genInterCode(IR_ASSIGN, place, t1);
        // }

        // Exp -> INT
        interCodeList->tempVarNum--;
        setOperand(place, OP_CONSTANT, (void*)atoi(node->child->val));
        // pOperand t1 = newOperand(OP_CONSTANT, node->child->val);
        // genInterCode(IR_ASSIGN, place, t1);
    }
}

void translateCond(pNode node, pOperand labelTrue, pOperand labelFalse) {
    assert(node != NULL);
    if (interError) return;
    // Exp -> Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | NOT Exp

    // Exp -> NOT Exp
    if (!strcmp(node->child->name, "NOT")) {
        translateCond(node->child->next, labelFalse, labelTrue);
    }
    // Exp -> Exp RELOP Exp
    else if (!strcmp(node->child->next->name, "RELOP")) {
        pOperand t1 = newTemp();
        pOperand t2 = newTemp();
        translateExp(node->child, t1);
        translateExp(node->child->next->next, t2);

        pOperand relop =
            newOperand(OP_RELOP, newString(node->child->next->val));

        if (t1->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        if (t2->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t2);
            t2 = temp;
        }

        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
    // Exp -> Exp AND Exp
    else if (!strcmp(node->child->next->name, "AND")) {
        pOperand label1 = newLabel();
        translateCond(node->child, label1, labelFalse);
        genInterCode(IR_LABEL, label1);
        translateCond(node->child->next->next, labelTrue, labelFalse);
    }
    // Exp -> Exp OR Exp
    else if (!strcmp(node->child->next->name, "OR")) {
        pOperand label1 = newLabel();
        translateCond(node->child, labelTrue, label1);
        genInterCode(IR_LABEL, label1);
        translateCond(node->child->next->next, labelTrue, labelFalse);
    }
    // other cases
    else {
        pOperand t1 = newTemp();
        translateExp(node, t1);
        pOperand t2 = newOperand(OP_CONSTANT, 0);
        pOperand relop = newOperand(OP_RELOP, newString("!="));

        if (t1->kind == OP_ADDRESS) {
            pOperand temp = newTemp();
            genInterCode(IR_READ_ADDR, temp, t1);
            t1 = temp;
        }
        genInterCode(IR_IF_GOTO, t1, relop, t2, labelTrue);
        genInterCode(IR_GOTO, labelFalse);
    }
}

void translateArgs(pNode node, pArgList argList) {
    assert(node != NULL);
    assert(argList != NULL);
    if (interError) return;
    // Args -> Exp COMMA Args
    //       | Exp

    // Args -> Exp
    pArg temp = newArg(newTemp());
    translateExp(node->child, temp->op);

    // if (temp->op->kind == OP_VARIABLE) {
    //     pItem item = searchTableItem(table, temp->op->u.name);
    //     if (item && item->field->type->kind == ARRAY) {
    //         interError = true;
    //         printf(
    //             "Cannot translate: Code containsvariables of "
    //             "multi-dimensional array type or parameters of array "
    //             "type when arg in function.\n");
    //         return;
    //     }
    // }
    addArg(argList, temp);

    // Args -> Exp COMMA Args
    if (node->child->next != NULL) {
        translateArgs(node->child->next->next, argList);
    }
}



