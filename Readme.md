# 0、安装

make install

会在当前目录安装，参考Makefile文件，主要是把.sh文件调整为可执行的，创建cache目录等。

# 一、头文件处理框架

### 1.在脚本开始位置通过.来引入include.h文件

```bash
#!/bin/bash
. $(dirname $0)/include.h
```

引入include.h就可以引入{include, ., source, ifndef, define, endif}关键字

### 2.编写自己的头文件

```bash
#!/bin/bash
ifndef __YOUR_H__
define __YOUR_H__
	...
else
	...
endif
```

头文件使用`ifndef ... define ... [else ...] endif`语法框起来，避免重复包含

### 3.把自己的头文件包含到脚本文件中

```bash
#!/bin/bash
. $(dirname $0)/include.h
include your.h
. your.h
source your.h
```

引用头文件有3种语法：

1\)  include [.h]

- 这种语法最简单，直接列出需要包含的头文件

2\)  . [.h] [args]

- 这种语法可以给头文件传递参数

3\)  source [.h]
- 这种语法头文件在执行时，会读取脚本的位置参数

  并可以修改脚本的位置参数，重设脚本的位置参数 
  典型用法：

  - `source options.h`

  解析脚本的参数，参考第二章

  使用这种用法需要在include.h中包含一个头文件的别名

  - `alias options.h='${SRC_DIR}/options.h'`

  不然会找不到需要包含的头文件的路径

### 4.允许多重包含

```bash
#!/bin/bash
ifndef __YOUR_H__
define __YOUR_H__
...
include other.h
...
endif
```

头文件内部可以包含其他头文件

# 二、命令行参数框架(options.h)

### 1.选项结构定义

```
Options=(
    "arg"     #非选项参数，需要脚本自己解析
    "desc"    #脚本的简单描述
             #下面是4个一组的选项描述
    "c"       #短选项，通常是一个字母，可以指定为空字符串表示没有短选项
    "name"    #长选项，也是解析后的变量名，必须有值，不可以为空。变量名中的-会被替换为_
    "has_arg" #表示选项是否带参数，Y:需要参数；N:不需要参数；O:可选参数
    "desc"    #选项的简短描述，支持反斜杠转义字符，如\033[7m
)
```

### 2.选项处理

```bash
#!/bin/bash
Options=(...)
...
source options.h
```

options.h脚本内部会解析每个选项

1\)如果是带参数的选项，则解析后会设置name变量的值

```bash
#!/bin/bash
if [ ! -v name ] ; then
    : #选项未设置
elif [ -z "$name" ] ; then
    : #选项设置，带参数，但参数为空
else
    : #选项设置，带参数，但参数不为空
fi
```

2\) 如果是不带参数的选项，指定该参数后，解析完name=Y，未指定该参数，不设置name变量

```bash
#!/bin/bash
if [ ! -v name ] ; then
    : #选项未设置
elif [ "$name" = "Y" ] ; then
    : #选项设置
fi
```

3\) 如果是可选参数的选项，不带参数时设置name="-"，带参数时设置name为参数值

可选参数且不带参数时使用下面的方法判断：

```bash
#!/bin/bash
if [ ! -v name ] ; then
    : #选项未设置
elif [ "$name" = "-" ] ; then
    : #选项设置，但不带参数
elif [ -z "$name" ] ; then
    : #选项设置，带参数，但参数为空
else
    : #选项设置，带参数，但参数不为空
fi
```

解析完的选项参数会被移除，最后剩余的非选项参数

### 3.demo.sh

```bash
#!/bin/bash
. $(dirname $0)/include.h
Options=(
    "<demo>"
    "example"
    "s"    "start"  "Y"  "start argument"
    "e"    "end"    "Y"  "end argument"
    ""     "none"   "N"  "no short option"
    "t"    "temp-y" "Y"  "带-的变量"   #解析完后变量名为temp_y
    "m"    "mem"    "O"  "option argument"
)
source options.h

echo $start $end $none $temp_y $mem
if [ -v mem -a -z "$mem" ] ; then
    echo "mem no argument"
fi
echo "$@"
```

### 4.可扩充选项头文件

```bash
#!/bin/bash
ifndef __YOUR_H__
define __YOUR_H__
Options=(
    "${Options[@]}"
    "e"    "end"    "Y"  "end argument"  #新增选项
)
else #可选的可以包含else
     #解析扩充的选项参数
endif
```
```bash
#!/bin/bash
. $(basedir $0)/include.h
Options=(
    ...
)
include your.h
source options.h
...
include your.h
```

脚本文件内包含your.h之后就可以看到扩充的选项

需要在解析options.h之前包含your.h

# 三、核心工具集(core.h)

### 1.基本工具集

1\) hmreadable bytes

-  把字节数转换为 KB MB GB TB PB EB


2\) hmreadable1 bytes

-  把字节数转换为 MB GB TB PB EB

3\) hmtobyte hm

-  把可阅读的字节单位转换为字节

4\) pexist ip process

-  判断ip节点上进程名为process的进程是否存在

5\) progress done total

-  进度条
   循环调整done的值，显示每一次的进度

6\) stdin_is_terminal,stdout_is_terminal,stderr_is_terminal

-  判断标准输出,标准输入,标准错误是否是终端

7\) dump_stack

-  打印shell的函数调用栈


# 四、数据库表转化为命令行

### 1.表配置结构定义

```bash
TblConfigure=(
    "tbl_name"      #定义表的名字
    "key"           #定义关键字段名，作为唯一查询键值
                #下面是3个一组的配置
    "c"             #短选项名字，通常是一个字母，可以指定为空字符串表示没有短选项
    "permit_set"    #是否允许修改字段值：Y,可以修改，可以查询；N,只能查询
    "field_name"    #数据库表字段的名字
)
```

### 2.解析选项

source tbl.h

包含tbl.h之后就可以了【目前支持查询数据库表的查询和更新】

附加的选项：

-  `-O, --omit-field       only print result, omit field's name`

只输出结果，不显示字段的名字

-  `--key                  specify key field (default: strvmname)`

指定新的键名字，替换默认的键

### 3.example

```bash
#!/bin/bash
. $(dirname $0)/include.h

TblConfigure=(
  tbl_vm
  strvmname
  u  N   uidvmid
  s  N   strvmname
  d  Y   strdesc
  f  N   strdiskfilename
)
source tbl.h
```

```bash
# ./demo-tbl.sh -u -s test1
# ./demo-tbl.sh -u -s --key uidvmid fac5-2b20e7996614f90cff4-
```

tbl.h 内会自动生成命令行选项，解析命令行参数，输出查询结果

test1 是web管理平台上看到的虚拟机的名称，或者瘦终端登录后看到的虚拟机名称

fac5-2b20e7996614f90cff4- 替换键为uidvmid来查询


# 五、处理二进制文件

### 1.转换二进制数据为结构体

```c
struct_name=(
    uint8_t     a
    uint16_t    b
    uint32_t    c
    uint64_t    d
    uint8_t[5]  e
    uint16_t[5] f
    uint32_t[5] g
    uint64_t[5] h
    uint8_t[a]  i
    uint16_t[b] j
    uint32_t[c] k
    uint64_t[d] l
    string      m
    struct other    n
    struct other[5] o
    dummy[5]     p
    dummy[a]     q
)
```

1\)结构体支持的数据类型：

```
uint8_t     1个字节整数
uint16_t    2个字节整数
uint32_t    4个字节整数
uint64_t    8个字节整数
uint8_t[5]  定长数组，数组长度最小为1，不能为0
uint8_t[a]  变长数组，数组长度由[]内的变量定义，变量必须提前赋值
string      字符串，结尾为0的字符串
struct struct_name 嵌入一个结构体
struct struct_name[5] 嵌入一个结构体数组，数组长度最小为1
dummy[5]    读取5个字节后丢弃
dummy[a]    变长丢弃，丢失字节数由[a]变量定义
```

2\)数据类型的大端和小端：

只有`uint8_t,uint16_t,uint32_t,uint64_t`四种基础类型支持大小端

小端：`uint8_t,uint16_t,uint32_t,uint64_t`

大端：`UINT8_T,UINT16_T,UINT32_T,UINT64_T`

变长数组和定长数组，也就支持大小端了UINT16_T[5]


### 2.定义结构体变量

```bash
struct struct_name var;
```

使用struct关键字来定义一个结构体变量

注意：

- 1\) 结构体名字和变量名字不能重名
  
- 2\) 结构体名字，变量名字，不能和shell的其他函数重名

### 3.结构体成员访问

1\)访问基础类型成员

```bash
${var[a]}  ${var[b]}  ${var[c]}  ${var[d]}
var.a = 4 $(var.a)
```

2\)访问数组类型成员(定长数组或变长数组)
```bash
${var[e[0]]}  ${var[e[1]]}  ${var[e[2]]}  ${var[e[3]]}  ${var[e[4]]}
var.e[0] = 5
```
成员名后加上数字来访问，第一个成员从0开始


3\)访问内嵌结构体的成员
```bash
${var[n.a]}  ${var[n.b]}
var[n.a]=4  var[n.b]=5
var.n.a = 4
```

4\)访问内嵌的结构体数组的成员
```bash
${var[o[0].a]}  ${var[o[4].b]}
var[o[0].a]=1  var[o[4].b]=4
var.o[0].a = 1
```

### 4.结构体赋初值

1\)定义变量时赋初值
```bash
struct struct_name var = { 1 2 3 4 }
```

2\)之后赋初值
```bash
var = { 1 2 3 4 }
```

### 5.从stdin读取结构体

1\)定义变量时直接读取
```bash
struct struct_name var _in_;
```

2\)变量定义后
```bash
struct_name var _in_;
```

or
```bash
var _in_;
```

### 6.把结构体输出到stdout
```bash
struct_name var _out_;
```

or
```bash
var _out_;
```

### 7.dump结构体
```bash
struct_name var _dump_;
```

or
```bash
var _dump_;
```

### 8.二进制文件的一般处理流程

1\)定义结构体

2\)从stdin不断的读取结构体

3\)用shell的语法进行处理，对结构体变量进行运算，过滤，数值处理等

4\)输出到stdout

### 9.内部原理

struct关键字是个shell函数，struct_name是个shell数组，包含结构体的基本类型。

由struct函数处理后，会生成一个以struct_name命名的新shell函数，于是以struct_name来定义变量时，相当于以变量名为参数来调用struct_name函数。

struct_name函数内部，会把$1(变量名)声明为全局的关联数组，同时会把结构体的类型，转化为各个类型函数：

```c
  uint8_t     name  =>  __uint8_t name i $@
  uint16_t    name  =>  __uint16_t name i $@
  uint32_t    name  =>  __uint32_t name i $@
  uint64_t    name  =>  __uint64_t name i $@
  uint8_t[5]  name  =>  __uint8_t[] 5 name i $@
  uint16_t[5] name  =>  __uint16_t[] 5 name i $@
  uint32_t[5] name  =>  __uint32_t[] 5 name i $@
  uint64_t[5] name  =>  __uint64_t[] 5 name i $@
  uint8_t[a]  name  =>  __uint8_t[] a name i $@
  uint16_t[b] name  =>  __uint16_t[] b name i $@
  uint32_t[c] name  =>  __uint32_t[] c name i $@
  uint64_t[d] name  =>  __uint64_t[] d name i $@
  string      name  =>  __string name i $@
  struct other name =>  __struct other name i $@
  struct other[5] name => __struct[] other 5 name i $@
  dummy[5]    name  =>  __dummy[] 5 name i $@
  dummy[a]    name  =>  __dummy[] a name i $@
```

结构体每个成员的类型，转化为类型函数的参数。

各个类型函数会处理其余的参数($2...$N)，同时会向关联数组内添加以name为键的值

给struct_name函数传递不同的变量名，就会得到不同的关联数组，就意味着定义了不同的结构体变量。

结构体内的元素出现的顺序不能混乱。

### 10.性能

用shell处理二进制文件的性能会比较差。

尤其是循环处理数组成员时，性能会急剧下降。

需要高性能处理的慎用此功能

### 11.未实现的功能

1\) 不能处理指针类似，基础类型指针、结构体指针、指针数组

2\) 未加入浮点类型、带符号的整数

3\) 不支持给数组类型赋值

4\) 赋值支持的不完善



# 六、类支持

### 1.定义类
```cpp
Class Human {
    public string name
    private uint32_t height
    Human::Human() { :; }
    Human::~Human() { :; }
    Human::speak() { echo hello world }
    Human::Introduce() {
        echo My name is $(Human::name), height $(Human::height)
    }
}
```
类使用关键字Class定义(首字母大写)，之后跟着类名字，花括号括起来的是类体

### 2.类对象

```cpp
class Human mark
```

通过`class`关键字来定义类对象。class首字母小写。

### 3.类成员变量

成员变量以`public`和`private`关键字来定义，同时指明成员变量的公有和私有，public成员变量，可以外部访问，private成员变量，只能在类体内访问。

支持的数据类型同结构体支持的类型。

1\)  通过类对象访问公有成员
```cpp
mark.name = "Nicolas.Mark"
```

2\)  类体内访问全部成员
```bash
Human::name = "Mark"    #给成员变量赋值
$(Human::name)          #读取成员变量
```
### 4.类成员函数

类成员函数以类名字加上::来定义。类似C++的类外定义成员函数一样。
```bash
Human::speak() { echo hello world }
```
Human类名，::作用域指示，speak函数名，()指明是函数


1\)  默认类成员函数都是public的，但可以使用private关键字把成员函数变成私有类型。
```bash
private speak   #把speak成员函数改变成private
```
改变成私有类型后，不能通过类对象在外部访问。    

2\)  通过类对象访问公有成员函数
```bash
mark.speak
```

3\)  类体内访问私有和共用成员函数
```bash
Human::speak
```

4\)  类成员函数内访问公有和私有类成员变量
```bash
Human::name  Human::height
```
### 5.类构造函数

构造函数类似C++的构造函数。跟类名字相同的成员函数被确定为构造函数。
```bash
Human::Human() { :; }
```

1\)  构造函数在定义类对象时调用。
```bash
class Human mark
```
会调用类的构造函数来初始化mark对象。
    

2\)  没有定义构造函数的类，默认会从stdin读取二进制数据来构造类对象。

-   这提供了一直极简的方式来处理二进制数据
    
-   如果不需要这种方式，则需要提供空构造函数
    

### 6.类析构函数

析构函数类似C++的析构函数。跟类名字相同的成员函数加上~被确定为析构函数。
```bash
Human::~Human() { :; }
```

1\)  析构函数在类对象释放时调用。
```bash
delete mark
```
释放mark对象，调用析构函数
    

2\)  没有定义析构函数的类，默认会把类串行化到stdout。

-   这提供了一直极简的方式来处理二进制数据
    
-   如果不需要这种方式，则需要提供空析构函数。或者不delete对象。
    

### 7.类对象的动态分配与释放

1\)  new 关键字可以动态分配类对象，使用`Object*`来调用。
```bash
Object* pobj = new Human
```
定义pobj指针来存放新分配的Human对象
    

2\)  delete 关键字删除静态分配的类对象。
```bash
class Human mark
delete mark
```
delete只能释放静态分配的对象。脚本暂不具备对象的动态管理。
    

3\)  delete\* 关键字删除动态分配的类对象。
```bash
delete* pobj
```
    释放对象


4\)  访问对象公有成员变量和公有成员函数
```bash
$pobj.name = "Mark"
$pobj.speak
$($pobj.speak)
```

### 8.类派生与继承

类派生是通过在类体内加入:关键字来指定派生类的。类似C++的派生语法
```bash
: public|private baseClass
```

1\)  公有派生
-   公有派生，则派生类的类对象，无法访问基类的私有成员，可以访问
    基类的公有成员。
    派生类的成员函数，也无法访问基类的私有成员，可以访问基类的公有
    成员。

2\)  私有派生
-   派生类的类对象，无法访问基类的公有和私有成员。
    派生类的成员函数，可以访问基类的公有成员，不能访问基类的私有成员

### 9.运算符重载

运算符重载类似C++的运算符重载。语法：

```cpp
Human:++ () { Human::height ++ }
```

重载函数是通过类名加一个冒号(:)加上重载的运算符来定义的。

运算符可以是一元的，二元的。对于一元运算符左侧运算对象以this指针方式传递，重载函数内使Human::height这样的方式类访问左侧对象的成员变量。

对于二元运算符左侧运算对象也是以this指针方式传递，右侧运算对象以函数参数方式传递（通过$1来访问），此时右侧运算对象只能通过公有的成员。

```cpp
Human:+= () {
    if $1 instanceof Human ; then
        Human::height += $($1.height)
    fi
}
```

$1引用的是右侧运算对象，$1.height将没办法访问私有成员。


支持重载的运算符：
```
 The following list of operators is grouped into levels 
 of equal-precedence operators.  The levels are listed
 in order of decreasing precedence.

    id++, id--         variable post-increment, post-decrement
    ++id, --id         variable pre-increment, pre-decrement
    -, +               unary minus, plus
    neg(!), compl(~)   logical and bitwise negation
    **                 exponentiation
    *, /, %            multiplication, division, remainder
    +, -               addition, subtraction
    lshift, rshift     <<, >> left and right bitwise shifts
    le, ge, lt, gt     <=, >=, <, > comparison
    ==, !=             equality, inequality
    and(&)             bitwise AND
    ^                  bitwise XOR
    or(|)              bitwise OR
    land(&&)           logical AND
    lor(||)            logical OR
    
    =, *=, /=, %=,
    +=, -=, lshe(<<=), rshe(>>=),
    ande(&=), ^=, ore(|=)      assignment
```

### 10.RTTI

1\)  instanceof 操作符能识别对象是否是类的实例

```bash
 if mark instanceof Human ; then
     echo yes
 fi
```

2\)  typeof 关键字能够获得对象的类
```bash
    typeof mark  => class Human
```

### 11.串行化

1\)  readin 关键字从stdin读取数据并初始化类成员变量

2\)  putout 关键字从stdout串行化输出类的成员变量

3\)  这两个函数只能在类成员函数内调用。


### 12.极简二进制处理框架

```bash
#!/bin/bash
. $(dirname $0)/include.h
include class.h
Class Backup {
    public uint32_t idx
    public uint32_t offset
    public uint32_t length
    private uint32_t reserved
}
class Backup bkup
[ $(bkup.idx) -eq 0 ] && exit 1
delete bkup
```

从stdin读取Backup结构后，判断idx不为0从stdout输出，


### 13.类中嵌入结构体

类中嵌入结构体和结构体中嵌入结构体是一样的调用方法。

因结构体不具备成员函数，如果嵌入的结构体是公有的，则可以通过类对象访问。

如果嵌入的结构体是私有的，则只能在类体内访问。

```bash
#!/bin/bash
. $(dirname $0)/include.h
include class.h
FiveSenses=(
    string eyes
    string lips
    string ears
    string tongue
    string nose
)
Class Human {
    public string name
    public uint32_t height
    public struct FiveSenses five
    Human::Human() { Human::five.eyes = "small" }
}
class Human mark
mark.five.eyes = "small"
```

类体内通过Human::five.eyes的方式来访问结构体成员

类对象通过mark.five.eyes的方式来访问公有的结构体成员

### 14.类中嵌入类

类比较复杂，嵌入的类可能有派生关系，可能有成员函数，可能有嵌入其他类，所以嵌入类是以引用的方式来访问的。

```bash
#!/bin/bash
. $(dirname $0)/include.h
include class.h
Class Stupid {
    private string _desc
    private uint32_t _value
    Stupid::Stupid() { :; }
    Stupid::~Stupid() { :; }
    Stupid::desc() {
        [ $# -eq 0 ] && {
            echo $(Stupid::_desc)
            return
        }
        Stupid::_desc = "$@"
    }
    Stupid::value() {
        [ $# -eq 0 ] && {
            echo $(Stupid::_value)
            return
        }
        Stupid::_value = $1
    }
}
Class Human {
    public string name
    public uint32_t height
    public class Stupid stupid
    Human::Human() { 
        $(Human::stupid).desc 'It is foolish to waste time.' 
    }
}
class Human mark
$(mark.stupid).value 67
```

在类体内以$(Human::stupid)的方式来对嵌入类对象解引用，然后再调用嵌入类对象的方法或成员变量。

类对象通过$(mark.stupid)的方式来对嵌入类对象解引用，然后再调用嵌入类对象的方法或成员变量。

### 15.性能增强

对于bash 4.4及以上的版本，会缓存class编译的信息。第二次运行脚本时速度会快。

4.4以下的bash版本，每次都会对类进行动态编译，运行速度上会有一点损失。

推荐使用bash 4.4版本。

### 16.未实现功能

1\)  不支持protected保护级别



# 七、装饰器

语法：
```bash
@ DECORATOR Func
```

使用DECORATOR装饰器函数来装饰Func函数

### 1.装饰器函数

1\)静态装饰器函数：

```bash
DECORATOR() {
    echo do something
    $@
    echo do something
}
```

装饰器函数内通过$@来调用原函数及传递参数

2\)动态装饰器函数：

```bash
DECORATOR() {
    __DECORATOR() {
        echo do something
        $@
        echo do something
    }
    __return=__DECORATOR
}
```

动态装饰器函数通过在装饰器内部定义一个新的函数，通过\_\_return变量返回这个新生成的函数作为装饰器函数。这个返回的装饰器函数被认为是动态生成的。

可以通过`eval "__DECORATOR() { ... }"`这样的方式动态生成，用同一个装饰器装饰不同的函数时，可以每次都返回不同的装饰器函数。

### 2.装饰器只能定义在Func函数的下方。

```bash
Func() {
    echo $@
}
@ DECORATOR Func
```

### 3.类的成员函数也可以使用装饰器

```bash
Human::Func() {
    echo $@
}
@ DECORATOR Human::Func
```

### 4.通用的shell命令也可以使用装饰器

```bash
@ DECORATOR ls
ls
@ DECORATOR cd
cd /home
```

### 5.可以在调用某函数前才对该函数进行装饰

必须保证装饰器只能调用一次，避免调用多次。

### 6.常用的装饰器
```bash
@logger  Func   打印日志
@runtime Func   打印运行时间
@disable Func   禁止函数运行
```

# 八、协程

在编写特殊脚本时，需要并发执行多个执行流，例如：伴随脚本执行的过程中，需要执行ping命令来持续测试网络延迟；脚本执行到某个阶段需要停止ping命令；之后获得ping命令的输出并分析结果。这就需要在特定的点起的协程，同时还能够与协程交互，杀死协程等。

目前支持的基本函数：

```bash
costart NAME << "EOF"				协程启动，执行一个命令体
    do somthing
EOF
costart NAME <<< do_function 		协程启动，执行一个函数
cokill NAME							杀死协程
cowait NAME							等待协程执行完毕
cocat NAME							获取协程数据
coread NAME							从协程读取一行
cowrite NAME <<<"write a line"		向协程输入一行
cowrite NAME << "EOF" 				向协程输入多行
    write multi-line
EOF
```

