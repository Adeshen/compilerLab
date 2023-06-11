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
* [X] 插入实现
* [X] 错误覆盖设计
* [X] 错误覆盖实现

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

**共有下列17种错误情况，大部分错误都是重复错误**

**可以仅仅通过查表进行解决**

```
//enum.hpp

typedef enum _errorType {
    UNDEF_VAR = 1,         // Undefined Variable
    UNDEF_FUNC,            // Undefined Function
    REDEF_VAR,             // Redefined Variable
    REDEF_FUNC,            // Redefined Function
    TYPE_MISMATCH_ASSIGN,  // Type mismatchedfor assignment.
    LEFT_VAR_ASSIGN,  // The left-hand side of an assignment must be a variable.
    TYPE_MISMATCH_OP,      // Type mismatched for operands.
    TYPE_MISMATCH_RETURN,  // Type mismatched for return.
    FUNC_AGRC_MISMATCH,    // Function is not applicable for arguments
    NOT_A_ARRAY,           // Variable is not a Array
    NOT_A_FUNC,            // Variable is not a Function
    NOT_A_INT,             // Variable is not a Integer
    ILLEGAL_USE_DOT,       // Illegal use of "."
    NONEXISTFIELD,         // Non-existentfield
    REDEF_FEILD,           // Redefined field
    DUPLICATED_NAME,       // Duplicated name
    UNDEF_STRUCT           // Undefined structure
} ErrorType;

//semantic_symbol_struct.h

static inline void pError(ErrorType type, int line, char* msg) {
    printf("Error type %d at Line %d: %s\n", type, line, msg);
}

```

## gdb debug

### 案例测试情况

test4.cmm 错误形式参数，当作未定义了                   解决

test5.cmm 输出差异                                                解决

test6 常量左值                                          解决

test9 还是 形参没有定义， 而且函数的参数没有读取到   50%   信息不全  解决

test13 直接爆出内存！                  50%      访问空   解决

test14 检测到语法错误！                    解决

test15 检测到重定义之后爆出内存     解决

test16 爆出内存                 解决

test17 爆出内存               解决

test_o1 test_2  函数声明冲突   解决

总共问题：爆出内存、检测到语法错误、常量左值未检查、赋值检测遗漏、函数形式参数未扫描到、函数报错信息不够详细，函数声明定义冲突

### 解决情况

* 问题1：形式参数未扫描到

原因：在语法分析的时候，有一个varList命名错误

* 问题2：赋值检测遗漏、常量左值未检查

原因：词法分析生成语法时，命名错误

* 问题3：检测到语法错误 真值表达式错误

原因：语法分析，关系运算符错误书写

* 问题4：爆出内存

原因：哈希函数，取余黑科技写错了，访问哈希表越界

* 问题5：释放空内存

原因：在非struct使用dot 之后delete了一次，之后又delete了一次

* 问题6：函数类型等价

原因：此前没有考虑过，函数可以声明，再定义；声明多次，

解决方案：增加函数类型比较机制

## 完整测试案例

**在** `Makefile`目录直接使用

`make test MAIN_CASE=test1.cmm`

**如果测试test2.cmm，就改为**

`make test MAIN_CASE=test2.cmm`

# lab3 中间代码翻译

## 需求分析

* [X] 设计存储中间代码的结构
* [X] 节点翻译函数
* [X] 虚拟机测试
* [X] 支持多维度数组
* [X] 支持结构体传参

## 设计代码结构体

每条中间代码，由两部分组成。

操作数+指令类型

```cpp
typedef enum _operand_kind{
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_LABEL,
        OP_FUNCTION,
        OP_RELOP,
} OpKind;


typedef enum _ir_kind{
        IR_LABEL,
        IR_FUNCTION,
        IR_ASSIGN,
        IR_ADD,
        IR_SUB,
        IR_MUL,
        IR_DIV,
        IR_GET_ADDR,
        IR_READ_ADDR,
        IR_WRITE_ADDR,
        IR_GOTO,
        IR_IF_GOTO,
        IR_RETURN,
        IR_DEC,
        IR_ARG,
        IR_CALL,
        IR_PARAM,
        IR_READ,
        IR_WRITE,
} IrKind;


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

```

不同的类型对应着不同翻译方案，具体的翻译在 `printInterCode`函数

## 翻译过程举例——多维数据进行距离

翻译的整体过程过于复杂，我以exp  这个最为齐全的概念进行举例

代码基本数据从何而来？

代码基本数据如何处理？

代码最终结果如何转化？

```cpp
pOperand newTemp() {
    // printf("newTemp() tempVal:%d\n", interCodeList->tempVarNum);
    char tName[10] = {0};
    sprintf(tName, "t%d", interCodeList->tempVarNum);
    interCodeList->tempVarNum++;
    pOperand temp = newOperand(OP_VARIABLE, newString(tName));
    return temp;
}
```

```cpp
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
                    pOperand offset = newTemp();
                    pOperand low_offset = newTemp();
                    pOperand width;
                    pOperand target;
                    //根据名字循环计算，每一层的偏移量，当层的偏移量存储在low_offset, 最终总的偏移存储在offset中。
                    while(top>0){
                        top--;
                        width = newOperand(
                            OP_CONSTANT, getSize(curType->u.array.elem));
                        genInterCode(IR_MUL, low_offset, idxs[top], width);
                        genInterCode(IR_ADD, offset, offset, low_offset);
                        curType=curType->u.array.elem;
                     
                    }
                    if (base->kind == OP_VARIABLE) {
                        // printf("非结构体数组访问\n");
                        target = newTemp();
                        genInterCode(IR_GET_ADDR, target, base);
                    } else {
                        // printf("结构体数组访问\n");
                        target = base;
                    }
                    //将基地址和偏移量相加得到最终地址
                    genInterCode(IR_ADD, place, target, offset);
                    place->kind = OP_ADDRESS;
                    interCodeList->lastArrayName = base->u.name;
                }
```

## 产生错误

### test2.cmm  function 参数传入时没有添加ARG

### test_o1.cmm  没有区分函数入参的结构体与函数内声明的结构体

 在传入的结构之时直接标记结构体为OP_ADDRESS, 如此就不会在运算成员地址时，重新取地址而是直接使用

### test_o2.cmm   while 无法识别

原来是词法识别时，while语句写错了名字，协程THILE

### test_o3.cmm  多维数组无法识别，已经修复

Cannot translate: Code containsvariables of multi-dimensional array type or parameters of array type

一开始并未处理多维数组的情况，

多为数组共出现在3个地方，

1.声明时

2.表达式中

3.传递参数中

### test_o4.cmm

Error type B at line 25: syntax error.
Error type B at line 30: syntax error.
Error type B at line 41: syntax error.

发现是词法分析中，没有单独识别 "&&"  "||"

只识别"|"  "&"

### test_o5.cmm

## 测试结果

安装测试工具

```bash
sudo apt-get install python3-pyqt5 python3-pyqt5.qtsvg

```

所有测试翻译结果都在文件夹Result3中，

* [ ] test1.cmm
* [ ] test2.cmm
* [ ] test_o1.cmm
* [ ] test_o2.cmm



# lab4 目标代码翻译

## 核心问题

1) 中间代码与目标代码之间并不是严格一一对应的。有可能某条中间代码对应多条目标 代码，也有可能多条中间代码对应一条目标代码。
2) 中间代码中我们使用了数目不受限的变量和临时变量，但处理器所拥有的寄存器数量 是有限的。RISC机器的一大特点就是运算指令的操作数总是从寄存器中获得。 
3) 中间代码中我们并没有处理有关函数调用的细节。函数调用在中间代码中被抽象为若 干条ARG语句和一条CALL语句，但在目标机器上一般不会有专门的器件为我们进行参数传 递，我们必须借助于寄存器或栈来完成这一点。



第一个问题被称为指令选择（Instruction Selection）问题，

第二个问题被称为寄存器分配（Register Allocation）问题，

第三个问题则需要考虑如何对栈进行管理。



在实验四 中我们的主要任务就是编写程序来处理这三个问题。





## 翻译方案



### 指令选择

有的时候为了得到更高效的目标代码，我们需要一次考察多条中间代 码，以期可以将多条中间代码翻译为一条MIPS32代码。这个过程可以看作是一个多行的模式 匹配，也可以看成用一个滑动窗口（Sliding Window）或一个窥孔（Peephole）滑过中间代码并查找可能的翻译方案的过程。这非常类似于我们课本上介绍的“窥孔优化”（Peephole  Optimization）的局部代码优化技术。

树形IR的翻译方式类似于线形IR，也是一个模式匹配的过程。不过我们需要寻找的模式不 再是一句句线形代码，而是某种结构的子树。树形IR的匹配与翻译算法被称为“树重写” （Tree-rewriting）算法，这在课本上也有介绍。



如何在中间代码中找到该图所对应的模式呢？答案是遍历。



### 朴素寄存器分配

RISC机器的一个很显著的特点是，除了load/store型指令之外，其余指令的所有操作数都 必须来自寄存器而不是内存。

除了数组和结构体必须放到内存中之外，中间代码里的任何一个 非零变量或临时变量，只要它参与运算，其值必须被载入到某个寄存器中。在某个特定的程序 点上选择哪个寄存器来保存哪个变量的值，这就是寄存器分配所要研究的问题。



朴素寄存器分配算法的思想最简单，也最低效：将所有的变量或临时变量都放在内存里。

寄存器分配之所以难是因为寄存器的数量有限，被迫共用同一寄存器的变量太多，导致这 些变量在使用时不得不在寄存器里换入换出，从而产生较大的访存开销。



我们考虑如何通过合 理安排变量对寄存器的共用关系来最大限度地减少寄存器内容换入换出的代价。有一种较好的 方法叫局部寄存器分配算法，该方法会事先将整段代码分拆成一个个基本块（将一段代码划分 成基本块的过程课本上有介绍，我们这里不再赘述），在每个基本块内部我们根据各种启发式 原则为块里出现的变量分配寄存器。



### 局部寄存器分配算法

其中，Free(r)表示将寄存器r标记为闲置。算法还用到另外两个辅助函数Ensure和 Allocate，它们的实现为：



上述算法的核心思想其实很简单：对基本块内部的中间代码逐条扫描，如果当前代码中有 变量需要使用寄存器，就从当前空闲的寄存器中选一个分配出去；如果没有空闲的寄存器，不 得不将某个寄存器中的内容写回内存（该操作称为溢出或spilling）时，则选择**那个包含本基 本块内将来用不到或最久以后才用到的变量的寄存器**。通过这种启发式规则，该算法期望可以 最大化每次溢出操作的收益，从而减少访存所需要的次数。

与缓存是一样的思路，和TLB也是一样的思路。



### 寄存器分配（图染色算法）

局部寄存器分配算法的这一弱点启发我们去寻找一个适用于全局的寄存器分配算法，这种 全局分配算法必须要能有效地从中间代码的控制流中获取变量的活跃信息，**而活跃变量分析 （Liveliness Analysis）恰好可以为我们提供这些信息**。



一个显而易见的寄存器分配原则就是，同时活跃的两个变量尽量不要分配 相同的寄存器。



1) 在赋值操作x := y中，即使x和y在这条代码之后都活跃，因为二者值是相等的，它们仍 然可以共用寄存器。
2) 









.

