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
|    multiprocessing     |   ❌    |   ✅   |  ✅   |
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

Say we have a file `MainArgs.cmm` with following contents:

```
void main(string args) {
    int i;
    for (i = 0; i < len(args); i = i + 1)
        println(i, args[i]);
}
```

By enter command `cmm MainArgs.cmm how are you`, the output on the screen will be

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
Haskell allow its user to create new operators. In CMM we have a similar feature.

Unlike the overloading of operators in C++, which allows users to define the behaviours
of operators on specific type, we let users use new symbols to create infix (binary)
operators and define the precedence. The definition of a customed operator is:

> infix [precedence] left-hand-side-operand operator-symbol  right-hand-side block

the precedence is optional and default to be 12. The operator symbol may begin with
a character in ``\@#$:`?``, and followed by arbitrary number of special characters.

Here's an example that defines an operator :+: which sums the natural numbers up
in range [lhs, rhs]:

```
infix low:+:high {
    int sum = 0, i;
    for (i = low; i <= hight; i = i+1) {
        sum = sum + i;
    }
    sum;
}
```

Then expresseion `1 :+: 100` will be evaluated to 5050.

When the block itself is an expression, we can express it in a more natural way:

> infix [precedence] lhs operator-symbol rhs = expression;

E.g., define a power operator with precedence 12: ``infix 12 x`^y = pow(x, y);``

### Dynamic Binding
Dynamic Binding is a feature similar to that in Emacs-Lisp.
Firstly, there're two concept that should be known before we go into detial:
_Lexical binding_ and _Dynamic binding_.

+ When you call a function with lexical binding, free variables in that function will
be looked up in the environment of its definition. It is also called "static binding"
because all bindings can be decided at compile time.
+ Dynamic binding means free variables in functions will be looked up for definition from
the environment where the function is invoked.

In CMM, functions called are by default static, which is also the choice of most
programming languages. If you want to force the intepreter to do a dynamic binding,
you can add a "!" between the function name and left parenthesis at call point.

An example:

```
int var = 1234;

void foo() { println(var); }

void main() {
    string var = "Hello";

    foo();   // Lexical binding
    foo!();  // Dynamic binding
}
```

The output would be: `1234  Hello`

## 2. The Interpreter
### Garbage Collection
CMM do garbeage collection by the reference counting algorithm.

Out implementation is pretty straightforward: all new objects are created by
`std::make_shared` and save them with smart pointer `std::shared_ptr<T>` in C++11, which
automatically manages the reference count and delete the object when refcount decreases to 0.

It is publicly known that there's a problem with reference counting algorithm: When objects
reference form a cycle, and the cycle cannot be used directly or indirectly from top level,
then they are leaked forever. We did not solve the problem and suggest avoid it in CMM.

A cycle reference example:

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

### Optimization
CMM implemented two simple optimization.

#### Constant Folding
*Constant folding* evaluate the value of constant expressions at compile time
as much as possible. We implemented a simple folding algorithm which, during the
creation of abstract syntax tree.

In CMM, statement `print(1 + 2 * 3)` will not be converted to a complex AST.
Instead, its AST will be equivalent to the one of `print(7)`.

#### Dead Code Elimination
*Dead Code Elimination* removes blocks in program which are unreachable.

In our implementation we did that for the most obvious dead code. This snippet
```
if ("hora" && 0.0)
    foo();
else
    bar();
```
will be replaced by a simple `bar()` invocation.


### 调用库函数
Whether a language is expressive or not is largely related to
whether it has sufficient library.
Users of CMM language can easily add system calls/library
function calls, as follows:

1. Add a line `ADD_FUNCTION(xxx)` in NativeFunctions.h, which is macro that expands to
the function prototype;
Write a wrapper function in NativeFunctions.cpp which wraps the library function:
It takes as input an array of `cvm::BaiscValue` and returns a `cvm::BasicValue`;
3. Register this function in NativeFunctionMap (in Interpreter.cpp)

## 3. The Editor

### Line Number Display
Display a line number at the left side and high light current line.

### Hightlight for Keywords
The reserved words in CMM are:

```
if else for while do break continue return int double
bool void string infix
```
Note: `do` is not yet used.

### File I/O

#### Create File / Open File (C+N/C+O)
If files are edited and not saved. When existing, a dialog will pop up
to confirm.
#### Save File (C+S)
The default is the Save option, and the newly created file is the Save As option.

### Run Script (C+R)
If the current editing file is not saved, you are prompted to save it.
On clicking the run button, terminal will started to compile and run.

### Error Message
Compilation errors and warnings are presented in the list.
The list contains incorrect rows, columns, and error messages.
Double-clicking on a line automatically jumps to the wrong location in the source code.


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
