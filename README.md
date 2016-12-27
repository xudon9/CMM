# The CMM Interpreter Project
2016 年 12 月 27 日

小组成员：王旭东、余璞轩、任屹


##0. Overview
+ 使用 C++11 开发，解释器本身只使用标准 C++ 库；
+ 跨平台，已测试平台包括 macOS、Linux、Windows；
+ 没有使用语法生成工具，词法器和语法分期全部采用手写；
+ 我们为之编写了一个专用代码编辑器，方便地编辑和运行 CMM 脚本；

| 功能或模块 \ 操作系统 | Windows | Linux | macOS |
|:------------------:|:-------:|:-----:|:-----:|
| CMM 语言基础功能 | ✅ | ✅ | ✅ |
| math 库调用     | ✅ | ✅ | ✅ |
| 多进程          | ❌ | ✅ | ✅ |
| Ncurses 库调用  | ❌ | ✅ | ✅ |
| GUI 文本编辑器   | 未测试 | 未测试 | ✅ |


##1. Language Design
我们小组实现的 CMM 语言，是一种有着 C 家族语言语法、内部解释机制接近 Python 的动态脚本语言。
另外也模仿了 Haskell 语言、Scala 语言、Lisp 语言的一些特性。

我们的 CMM 语言的语法与任务书要求并不完全一致，
在大体兼容原有语法的基础上增加了很多新特性。详情见附录 [CMM语言语法](#bnf)一节。

###基础语法

###类型系统与数组
CMM 语言包括四种基本数据类型：`int`，`double`，`bool`，`string`。数组声明方式与C语言相同，语句
>Type Identifier [Expr<sub>1</sub>][Expr<sub>2</sub>]...[Expr<sub>n</sub>] 

声明了一个元素类型为 T 的 n 维数组，名为 Identifier。他的第 i 维下标的有效范围是 [0, Expr<sub>i</sub> - 1]。

**注意：**与标准原始要求不同的，Expr 不需要是常量表达式。

隐式转换规则：

+ `int`与`double`操作时，会被提升为`double`
+ 与字符串相加的操作数会被转换为字符串
+ 元素类型为 T 的数组可以直接赋值给 T 类型变量（即 T 类型变量提升为 T 类型数组）
+ 逻辑运算符将所有操作数转换为布尔值。整数和浮点数的 0 以及空字符串视为 `false`，其他值视为为 `true`

###main 函数与命令行参数
CMM 语言可以定义一个可选的 main 函数。如果 main 函数存在，那么在顶层语句运行结束后再开始运行 main 函数。

main 函数的参数列表可为空，或单个`string`类型的形参。
它可以返回任意类型 (推荐返回`int`或`void`)，其他类型将会动态转成整数返回给操作系统。返回 0 代表程序执行成功，任意非零值表示失败。

下面是 main 函数的合法定义的一个简单例子：

```
int main(string args)
{
    if (len(args) < 1)
        retrun -1;
    println("Hello " + args[0]);
    return 0;
}
```

其中`string`类型的参数是一个包含命令行参数的数组，参数个数可用`len`函数计算得到。

例如，假设有文件 `MainArgs.cmm` 代码如下:

```
void main(string args) {
    int i;
    for (i = 0; i < len(args); i = i + 1)
        println(i, args[i]);
}
```
执行命令 `cmm MainArgs.cmm how are you` 的结果为:

```
0 how
1 are
2 you
```
###默认返回值
此特性模仿自 Scala 语言。函数体或中缀操作符语句内的最后被执行的表达式被当做它的返回值。例如，求两个整数的最大值可以以这种简洁的方式书写：

```
int max(int a, int b) {
    if (a>b) a; else b;
}
```
甚至更简单些：
```
int max(int a, int b) if (a>b) a; else b;
```
###自定义操作符
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

###动态绑定(Dynamic Binding)
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

##2. The Interpreter
###垃圾回收
CMM 采用引用计数算法进行垃圾回收。

我们的 C++ 实现方法是：采用 std::shared_ptr&lt;T&gt; 智能指针代表引用。当指针被赋值时，它指向的原对象计数减 1，新指向的对象计数加 1。
当引用计数减到 0 时，对象被析构。

众所周知，引用计数算法有一个明显的缺陷：对象之间循环引用时，脱离引用范围的环形对象无法回收。
例如：

```
if (true) {
  int A[3];   // new array object referenced by A
  A[0] = A;   // new array object referenced by A and its first element
}

/*
 * Now A is not in scope, but the array is still referenced by 
 * its first element. We lost it forever.
 */
```

###编译优化
CMM 解释器实现了两种常见编译优化算法的简单版本。
####常量折叠(Constant folding)
*常量折叠*指在编译时期简化常数的过程，常数在表示式中仅仅代表一个简单的数值。

CMM 解释器中，`print(1 + 2 * 3)` 将不会被转换成一个复杂的语法树，
而是被折叠为等价于 `print(7)` 这样的简单调用。
####死代码消除(Dead code elimination)
*死代码消除*值移除对程序运行结果没有任何影响的代码。

CMM 解释器实现了一些最基本的死代码消除算法。例如下面的 CMM 代码：

```
if ("hora" && 0.0)
    foo();
else
    bar();
```
会被解释器简化成等价于 `bar();` 的代码。

-------------------------------------------------
##附录
###CMM双目运算符优先级表
| 运算符                                   |优先级|
|:---------------------------------------:|:---:|
| = (赋值操作)                             | 1 |
| &#124;&#124; (逻辑或)                    | 2 |
| &amp;&amp; (逻辑与)                      | 3 |
| &#124; (按位或)                          | 4 |
|^ (按位异或)                              | 5 |
|&amp; (按位与)                            | 6 |
|== (相等) != (不相等)                      | 7 |
|< (小于) <= (小于等于) > (大于) >= (大于等于)| 8 |
|<< (按位算数左移) >> (按位算数右移)          | 9 |
|+ (加法) - (减法)                         | 10 |
|* (乘法) / (除法) % (取余)                 | 11 |
|   用户自定义双目操作符的默认优先级           | 12 |

###<a name="bnf"></a>CMM语言语法

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

###构建CMM解释器程序的方法
1.  `D:\Project\cmm`
2. Go to `D:\Project\cmm`, and create a new folder/directory `build`,
	namely `D:\Project\cmm\build`
3. Go to the `build` directoy you created, run `cmake .. -G "Visual Studio 14"`
   if you're using
	Visual Studio 2015. If you use UNIX, run `cmake .. -G "Unix Makefiles"`
4. Build the project with the generated Visual Studio project / Makefile.
