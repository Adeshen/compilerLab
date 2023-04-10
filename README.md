# lab1 基础语法树的建立

## 任务目标：

* [X] token的识别
* [X] 文法规则的设计
* [X] 错误的检测

## 感悟

基础项目，重点学习Makefile编译多个CPP文件

# lab2 语义分析——装饰语法树的建立

## 需求分析

* [X] 符号表结构的设计
* [X] 表删、查、改函数实现
* [X] 各个词法单元的插入设计
* [ ] 插入实现
* [ ] 错误覆盖设计
* [ ] 错误覆盖实现

要求1：使其变为“函数除了在定义之外还可以进行声明”。函数的定义仍然不可以重复出现，但函数的声明在相互一致的情况下可以重复出现

要求2：使其变为“变量的定义受可嵌套作用域的影响，外层语句块中定义的变量可在内层语句块中重复定义（但此时在内层语句块中就
无法访问到外层语句块的同名变量），内层语句块中定义的变量到了外层语句块中就会消亡，不同函数体内定义的局部变量可以相互重名”。

要求3：修改前面的C−−语言假设5，将结构体间的类型等价机制由名等价改为结构等价（Structural Equivalence）。

例如，虽然名称不同，但两个结构体类型

```
struct a { int x; float y; }和struct b { int y; float z; }仍然是等价的

struct A { int a;struct { float f; int i; } b[10]; }

和struct B { struct { int i; floatf; } b[10]; int b;}是不等价的。
```

## 符号表设计

符号表：使用动态更新的单表
数据结构：哈希表+作用域栈

哈希表：

* 使用 堆内存存储
* 哈希冲突解决：拉链法
* 哈希函数：幂权重 字符

作用域栈：

* 作用域深度：从初始头部0 全局1 开始，每一个栈代表一个深度的所有符号链
* 插入时机：将一个符号元素、加入表的时候，哈希表与栈同时加入
* 删除时机：离开作用域时

item四种类型设计：

* 基本类型：直接一个结构体type可以表示的
* 数组：大小+类型
* 函数：输入参数+返回类型
* 复合结构： 结构体名字+成员链表

## 词法单元插入设计

所有词法单元插入函数都遵顼类似的设计，如下面FunDec函数一样

### 基本流程

1. 创建newItem()
2. 分割各个域名，分别用对应的插入函数处理
3. 判空之后加入符号表

```cpp
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
```

### 作用域深度变化情况

其中涉及一个不同作用域更新的问题，只有

这个{} 才会清空当前作用域

```cpp
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
```

而且也会更新作用域。

当然除了这个会增加作用域深度之外还有一个东西会增加作用域的深度。

函数的形式参数定义

```cpp
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
```

## 错误覆盖

### 函数参数匹配
