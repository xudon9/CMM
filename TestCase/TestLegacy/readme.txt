/** 
 *Title 测试用例说明
 *Author 邱忠磊
*/
 测试用例说明：
【1】本测试用例应该要正确执行的主要分为以下9个模块：
	test1——变量声明
	test2——一般变量赋值
	test3——数组
	test4——算术运算
	test5——IF-ELSE
	test6——WHILE
	test7——IF-ELSE与WHILE
	test8——阶乘
	test9——数组排序
     报错的用例则有下面3个模块
	error1——ID		测试标志符命名和变量作用域
	error2——array		测试数组越界和下标非法
	error3——comment	测试注释嵌套和多行注释无结尾

因各同学对于条件判断的定义各有不同，所以只采用了基
本的关系运算<、==和<>。

【2】每个用例中都带有单行注释、多行注释和write语句，
并且在数组和一般变量赋值用例中使用了read语句，因此
并没有再对注释、read、write等进行单独测试，

【3】在本用例中，将write和read语句的语法定义为
 write(变量|expresion) 和 read(变量) 的形式，如果有
同学定为 write expression|变量 或者 read 变量 或者
 变量= read() 的形式则要对用例进行更改。
