# The CMM Interpreter Project

Dec. 27, 2016

Group Memers: Xudong Wang, Puxuan Yu.


## 0. Overview

+ The project was implemented in C++11. No 3rd party libraries are used in
the intepreter core;
+ Its corss-platform. We have tested it under macOS, Linux, and Windows;
+ No parser generators (e.g. Bison, Antlr) were used. The lexer and parser were
purely hand-written for more flexibiliy and more user-friendly error message;
+ A GUI code editor was provided to let users edit and run CMM scripts more conveniently.

| Modules \ OS | Windows | Linux  | macOS |
|:---------------------:|:-------:|:------:|:-----:|
|    CMM Language core   |   ✅    |   ✅   |  ✅   |
|    math library        |   ✅    |   ✅   |  ✅   |
|    multiprocessing             |   ❌    |   ✅   |  ✅   |
|    Ncurses library     |   ❌    |   ✅   |  ✅   |
|    GUI code editor     | Not tested  | Not tested |  ✅   |


## 1. Language Design
The laguagewe implemented, named CMM, has a grammar similar to C family languages,
    but its internal mechanism is close to Python. Some features from Haskell, Scala, and
    Lisp are also borrowed.

The grammar of CMM is not exact identical to what is required in the task specification.
All required syntax are supported by our implementation, in the meanwhile, many new features
are supported. The detailed grammar can be checkout out at section [CMM Grammar BNF](#bnf).

### Basic Syntax
The basic syntax of CMM is very similar to C. Here are some points worthy to know:

+ Empty statements are allowed (namely a single semicolon as statement). A warning message
will be generated upon empty statements;
+ Like C, the expressions in `for` loops may be omitted. E.g., a common way to write a infinite
loop is `for (;;) {}`
+ Boolean logic operators are short-circuited. Consider the following snippet:

```C
bool foo() { println("hello"); return true; }
bool bar() { println("world"); return false; }

if (foo() || bar)
    ;
```
The output will be "hello".

### Type System & Arrays
There are 4 primitive types in CMM: `int`, `double`, `bool`, `string`.
The declaration of arrays are similar to C. The following statement
> Type Identifier [Expr<sub>1</sub>][Expr<sub>2</sub>]...[Expr<sub>N</sub>]

delcares an N-dimensional array of type T with name Identifier. Its valid range
of the i<sup>th</sup> index is [0, Expr<sub>i</sub> - 1].

**Note:** Different from the required of the assignment and the rule in C89, the declaration
of arrays do not require size expression to be constant.

Implicit Conversion Rules:

+ When operand of a operator are `int` and `double`, the integer will be promoted to `double`;
+ If any operator of `+` is a string, then another operand will be converted to string;
+ Arrays of type T are considered to be a subtype of T. That is to say, a variable of type T
can store array of that type;
+ All operands of logical operators will be converted to boolean values. E.g., numerisc zeros
and empty strings are `false`, otherwise `true`.

### The 'main' Function & Command Line Arguments
`main` function are optional in CMM. If the programmer defined such a function, then it will
be invoked after all top-level statements and definitions executed.

The parameter list of `main` can be empty, or it can take a `string` as input.

The `main` function may return arbitrary type (although `int` and `void` are recommended).
returned value will be converted to integers and be will returned to the operating system.
As a convention, a zero value indicates successful execution. Non-zero values means failure.

Here's a simple example of a valid definition of the main function:

```
int main(string args)
{
    if (len(args) < 1)
        return -1;
    println("Hello " + args[0]);
    return 0;
}
```

In this example, the `args` of is a string array (remember that T arrays are subtype of T).
The length of command line arguments can be calculated by function `len`.

Say we have a file `MainArgs.cmm` with following contents

```
void main(string args) {
    int i;
    for (i = 0; i < len(args); i = i + 1)
        println(i, args[i]);
}
```

By enter command `cmm MainArgs.cmm how are you`, the output on screen will be

```
0 how
1 are
2 you
```

### Default Return Value
In many languages like Scala and Ruby, the value of last statement or expression
that was executed in a function will be the default return value of it.
In CMM the we implemented the same feature.

E.g., a function that returns the maximum value can be written as:

```
int max(int a, int b) {
    if (a>b) a; else b;
}
```

Or even more concise:

```
int max(int a, int b) if (a>b) a; else b;
```

### User-Defined Operators
此特性模仿自 Haskell 语言。
与 C++ 中的操作符重载不同，自定义操作符允许用户使用新的符号作为中缀运算符，并且可以指定其优先级。其语法是：
> infix [优先级] 左操作数 运算符 右操作数 语句块

其中优先级是可选的，缺省值优先级 12。运算符以``\@#$:`?``中某个字符开始，后面可以接任意多个特殊字符。

下面是一个例子，定义符号`:+:`为求区间内所有整数之和”：

```
infix low:+:high {
    int sum = 0, i;
    for (i = low; i <= hight; i = i+1) {
        sum = sum + i;
    }
    sum;
}
```
则表达式`1:+:100`的值为 5050。
当语句块本身是一个表达式时，可以使用一种更接近数学的格式写，它更直观：
> infix [优先级] 左操作数 运算符 右操作数 = 表达式;

例如定义优先级为 12 的幂运算：``infix 12 x`^y = pow(x, y);``

### 动态绑定(Dynamic Binding)
此特性模仿自 Lisp 语言。
首先介绍两个概念：_Lexical binding_ 和 _Dynamic binding_。

+ Lexical binding 指调用函数时，对于函数内未定义的标识符，在函数**定义处**的环境中查找。由于函数定义处是确定的，因此这种绑定方式也叫静态绑定(Static binding)。
+ Dynamic binding 则指对于函数内未定义的标识符，在函数**调用处**的环境中查找。

在 CMM 语言中，调用函数时默认使用静态绑定，这也是大多数语言的选择。
如果想强制使用动态绑定，可通过在调用函数时在函数名后加感叹号“!”来实现。
例：

```
int var = 1234;

void foo() { println(var); }

void main() {
    string var = "Hello";

    foo();   // Lexical binding
    foo!();  // Dynamic binding
}
```
输出结果是：`1234  Hello`

## 2. The Interpreter
### 垃圾回收
CMM 采用引用计数算法进行垃圾回收。

我们的 C++ 实现方法是：采用 std::shared_ptr&lt;T&gt; 智能指针代表引用。
当指针被赋值时，它指向的原对象计数减 1，新指向的对象计数加 1。
当引用计数减到 0 时，对象被析构。

众所周知，引用计数算法有一个明显的缺陷：对象之间循环引用时，脱离引用范围的环形对象无法回收。
例如：

```C++
{
  int A[3];   // new array object referenced by A
  A[0] = A;   // new array object referenced by A and its first element
}

/*
 * Now A is not in scope, but the array is still referenced by 
 * its first element. We lost it forever.
 */
```

### 编译优化
CMM 解释器实现了两种常见编译优化算法的简单版本。
#### 常量折叠(Constant folding)
*常量折叠*指在编译时期简化常数的过程，常数在表示式中仅仅代表一个简单的数值。

CMM 解释器中，`print(1 + 2 * 3)` 将不会被转换成一个复杂的语法树，
而是被折叠为等价于 `print(7)` 这样的简单调用。
#### 死代码消除(Dead code elimination)
*死代码消除*值移除对程序运行结果没有任何影响的代码。

CMM 解释器实现了一些最基本的死代码消除算法。例如下面的 CMM 代码：

```
if ("hora" && 0.0)
    foo();
else
    bar();
```
会被解释器简化成等价于 `bar();` 的代码。

### 调用库函数
一门语言强大与否，和它是否有充足的库调用有紧密联系。
CMM 语言可以很方便地增加系统调用/库函数调用，步骤如下：

1. 在 NativeFunctions.h 增加一行 `ADD_FUNCTION(xxx)`，它是一个展开后得到函数原型的宏；
2. 在 NativeFunctions.cpp 里写一个包装函数，把库函数封装起来，得到一个接收 `cvm::BaiscValue` 列表并返回
   这种类型的函数
3. 在 Interpreter.cpp 中向 NativeFunctionMap 注册该函数

## 3. The Editor

### 行号显示
当前编辑行高亮

### 保留关键字高亮
以下是CMM的保留字：

```
if else for while do break continue return int double
bool void string infix
```
注：`do` 关键字暂时没有用到

### 文件I/O

#### 新建文件/打开文件(C+N/C+O)
如果当前有编辑后未保存的文件会提示保存。
#### 保存文件(C+S)
默认为“保存”选项，新创建的文件则为“另存为”选项。

### 一键运行(C+R)
当前编辑文件未保存则提示保存。
弹出 terminal 进行编译。

### Error Message
编译出的错误、警告分别呈现在列表当中。
列表包含错误的行、列和错误信息。
双击某行可以自动跳转到源码中的错误位置。


-------------------------------------------------
## Appendixes

### The precedence of operators in CMM
|                   Operator                    |Precedence|
|:-------------------------------------------:|:----:|
| = (assignment)                                |   1  |
|&#124;&#124; (logical or)                        |   2  |
|&amp;&amp; (logical and)                          |   3  |
|&#124; (bitwise or)                              |   4  |
|^ (bitwise exclusive or)                                 |   5  |
|&amp; (bitwise and)                               |   6  |
|== (equal to) != (not equal to)                        |   7  |
|< (less than) <= (less or equal than) > (greater than) >= (greater or equal than)|   8  |
|<< (bitwise leftshift) >> (bitwise rightshift)          |   9  |
|+ (addition) - (substraction)                            |  10  |
|&#42; (multiplication) / (division) % (modulo)               |  11  |
|The default precedence of user-defined binary operators             |  12  |

### CMM Built-in functions

Those functions are always available in CMM regardless of the operating system:

```
typeof
len
strlen
print
println (alias of puts)
system
random
rand
srand
time
exit
toint
todouble
tostring (alias of str)
tobool
read
readln
readint
sqrt
pow
exp
log
log10
```

The following functions are only available under Linux and macOS:

```
UnixFork
NcEndWin
NcInitScr
NcNoEcho
NcCursSet
NcKeypad
NcTimeout
NcGetCh
NcMvAddCh
NcMvAddStr
NcGetMaxY
NcGetMaxX
NcStartColor
NcInitPair
NcAttrOn
NcAttrOff
NcColorPair
```

### <a name="bnf"></a>CMM Grammar BNF

```
Program ::= TopLevel*

TopLevel ::= infixOperatorDefinition
TopLevel ::= functionDeclaration
TopLevel ::= DeclarationStatement
TopLevel ::= Statement

infixOpDefinition ::= Kw_infix [Integer] Id infixOp Id Statement
infixOpDefinition ::= Kw_infix [Integer] Id infixOp Id ["="] ExprStatement

functionDefinition ::= typeSpecifier identifier _functionDefinition
functionDefinition ::= typeSpecifier identifier _functionDefinition

_functionDefinition ::= "(" ")" Statement
_functionDefinition ::= "(" parameterList ")" Statement

parameterList ::= "void"
parameterList ::= TypeSpecifier Identifier ("," TypeSpecifier Identifier)*

block ::= "{" statement* "}"

typeSpecifier ::= "bool" | "int" | "double" | "void" | "string"

OptionalArgList ::= epsilon
OptionalArgList ::= argumentList

argumentList ::= Expression ("," Expression)*

EmptyStatement ::= ";"

Statement ::= Block
Statement ::= IfStatement
Statement ::= WhileStatement
Statement ::= ForStatement
Statement ::= ReturnStatement
Statement ::= BreakStatement
Statement ::= ContinueStatement
Statement ::= EmptyStatement
Statement ::= DeclarationStatement
Statement ::= ExprStatement

expression ::= primaryExpr BinOpRHS*

parenExpr ::= "(" expression ")"

primaryExpr ::= parenExpr
primaryExpr ::= identifierExpr
primaryExpr ::= identifierExpr ("[" Expression "]")+
primaryExpr ::= constantExpr
primaryExpr ::= ("~" | "+" | "-" | "!") primaryExpr

identifierExpression ::= identifier
identifierExpression ::= identifier  "("  optionalArgList  ")"

constantExpr ::= IntExpression
constantExpr ::= DoubleExpression
constantExpr ::= BoolExpression
constantExpr ::= StringExpression

ifStatement ::= "if" "(" Expr ")" Statement
ifStatement ::= "if" "(" Expr ")" Statement "else" Statement

forStatement ::= "for" "(" Expr ";" Expr ";" Expr ")" Statement

whileStatement ::= "while"  "("  Expression  ")"  Statement

exprStatement ::= Expression ";"

returnStatement ::= "return" ";"
returnStatement ::= "return" Expression ";"

breakStatement ::= "break" ";"

continueStatement ::= "continue" ";"

DeclarationStatement ::= TypeSpecifier _DeclarationStatement

_DeclarationStatement ::= SingleDeclaration+

SingleDeclaration ::= identifier "=" Expression
SingleDeclaration ::= identifier ("[" Expression "]")+
```
