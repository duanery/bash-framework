# һ��ͷ�ļ��������

### 1.�ڽű���ʼλ��ͨ��.������include.h�ļ�

```bash
#!/bin/bash
. $(dirname $0)/include.h
```

����include.h�Ϳ�������{include, ., source, ifndef, define, endif}�ؼ���

### 2.��д�Լ���ͷ�ļ�

```bash
#!/bin/bash
ifndef __YOUR_H__
define __YOUR_H__
	...
else
	...
endif
```

ͷ�ļ�ʹ��
   `ifndef ... define ... [else ...] endif`
�﷨�������������ظ�����

### 3.���Լ���ͷ�ļ��������ű��ļ���

```bash
#!/bin/bash
. $(dirname $0)/include.h
include your.h
. your.h
source your.h
```

����ͷ�ļ���3���﷨��
1)  include [.h] 
    �����﷨��򵥣�ֱ���г���Ҫ������ͷ�ļ�
2)  . [.h] [args]
    �����﷨���Ը�ͷ�ļ����ݲ���
3)  source [.h]
    �����﷨ͷ�ļ���ִ��ʱ�����ȡ�ű���λ�ò���
    �������޸Ľű���λ�ò���������ű���λ�ò���
    �����÷���
        source options.h
        �����ű��Ĳ������ο��ڶ���
    ʹ�������÷���Ҫ��include.h�а���һ��ͷ�ļ��ı���
        alias options.h='${SRC_DIR}/options.h'
    ��Ȼ���Ҳ�����Ҫ������ͷ�ļ���·��

### 4.�������ذ���

```bash
#!/bin/bash
ifndef __YOUR_H__
define __YOUR_H__
...
include other.h
...
endif
```

ͷ�ļ��ڲ����԰�������ͷ�ļ�

# ���������в������(options.h)

### 1.ѡ��ṹ����

```
Options=(
    "arg"     #��ѡ���������Ҫ�ű��Լ�����
    "desc"    #�ű��ļ�����
             #������4��һ���ѡ������
    "c"       #��ѡ�ͨ����һ����ĸ������ָ��Ϊ���ַ�����ʾû�ж�ѡ��
    "name"    #��ѡ�Ҳ�ǽ�����ı�������������ֵ��������Ϊ�ա��������е�-�ᱻ�滻Ϊ_
    "has_arg" #��ʾѡ���Ƿ��������Y:��Ҫ������N:����Ҫ������O:��ѡ����
    "desc"    #ѡ��ļ��������֧�ַ�б��ת���ַ�����\033[7m
)
```

### 2.ѡ���

```bash
#!/bin/bash
Options=(...)
...
source options.h
```

options.h�ű��ڲ������ÿ��ѡ��
1)����Ǵ�������ѡ�������������name������ֵ

```bash
#!/bin/bash
if [ ! -v name ] ; then
    : #ѡ��δ����
elif [ -z "$name" ] ; then
    : #ѡ�����ã���������������Ϊ��
else
    : #ѡ�����ã�����������������Ϊ��
fi
```

2) ����ǲ���������ѡ�ָ���ò����󣬽�����name=Y��δָ���ò�����������name����

```bash
#!/bin/bash
if [ ! -v name ] ; then
    : #ѡ��δ����
elif [ "$name" = "Y" ] ; then
    : #ѡ������
fi
```

3) ����ǿ�ѡ������ѡ���������ʱ����name="-"��������ʱ����nameΪ����ֵ
��ѡ�����Ҳ�������ʱʹ������ķ����жϣ�

```bash
#!/bin/bash
if [ ! -v name ] ; then
    : #ѡ��δ����
elif [ "$name" = "-" ] ; then
    : #ѡ�����ã�����������
elif [ -z "$name" ] ; then
    : #ѡ�����ã���������������Ϊ��
else
    : #ѡ�����ã�����������������Ϊ��
fi
```

�������ѡ������ᱻ�Ƴ������ʣ��ķ�ѡ�����

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
    "t"    "temp-y" "Y"  "��-�ı���"   #������������Ϊtemp_y
    "m"    "mem"    "O"  "option argument"
)
source options.h

echo $start $end $none $temp_y $mem
if [ -v mem -a -z "$mem" ] ; then
    echo "mem no argument"
fi
echo "$@"
```

### 4.������ѡ��ͷ�ļ�

```bash
#!/bin/bash
ifndef __YOUR_H__
define __YOUR_H__
Options=(
    "${Options[@]}"
    "e"    "end"    "Y"  "end argument"  #����ѡ��
)
else #��ѡ�Ŀ��԰���else
     #���������ѡ�����
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

�ű��ļ��ڰ���your.h֮��Ϳ��Կ��������ѡ��
��Ҫ�ڽ���options.h֮ǰ����your.h

# �������Ĺ��߼�(core.h)

### 1.�������߼�

1) hmreadable bytes
   ���ֽ���ת��Ϊ KB MB GB TB PB EB
2) hmreadable1 bytes
   ���ֽ���ת��Ϊ MB GB TB PB EB
3) hmtobyte hm
   �ѿ��Ķ����ֽڵ�λת��Ϊ�ֽ�
4) pexist ip process
   �ж�ip�ڵ��Ͻ�����Ϊprocess�Ľ����Ƿ����
5) progress done total
   ������
   ѭ������done��ֵ����ʾÿһ�εĽ���
6) stdin_is_terminal,stdout_is_terminal,stderr_is_terminal
   �жϱ�׼���,��׼����,��׼�����Ƿ����ն�
7) dump_stack
   ��ӡshell�ĺ�������ջ

# �ġ����ݿ��ת��Ϊ������

### 1.�����ýṹ����

```bash
TblConfigure=(
    "tbl_name"      #�����������
    "key"           #����ؼ��ֶ�������ΪΨһ��ѯ��ֵ
                #������3��һ�������
    "c"             #��ѡ�����֣�ͨ����һ����ĸ������ָ��Ϊ���ַ�����ʾû�ж�ѡ��
    "permit_set"    #�Ƿ������޸��ֶ�ֵ��Y,�����޸ģ����Բ�ѯ��N,ֻ�ܲ�ѯ
    "field_name"    #���ݿ���ֶε�����
)
```

### 2.����ѡ��

source tbl.h
����tbl.h֮��Ϳ����ˡ�Ŀǰ֧�ֲ�ѯ���ݿ���Ĳ�ѯ�͸��¡�
���ӵ�ѡ�
  -O, --omit-field       only print result, omit field's name
ֻ������������ʾ�ֶε�����
  --key                  specify key field (default: strvmname)
ָ���µļ����֣��滻Ĭ�ϵļ�

### 3.example

```bash
#!/bin/bash
. $(dirname $0)/include.h
|
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
tbl.h �ڻ��Զ�����������ѡ����������в����������ѯ���
test1 ��web����ƽ̨�Ͽ���������������ƣ��������ն˵�¼�󿴵������������
fac5-2b20e7996614f90cff4- �滻��Ϊuidvmid����ѯ

# �塢�����������ļ�

### 1.ת������������Ϊ�ṹ��

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

1)�ṹ��֧�ֵ��������ͣ�

```
uint8_t     1���ֽ�����
uint16_t    2���ֽ�����
uint32_t    4���ֽ�����
uint64_t    8���ֽ�����
uint8_t[5]  �������飬���鳤����СΪ1������Ϊ0
uint8_t[a]  �䳤���飬���鳤����[]�ڵı������壬����������ǰ��ֵ
string      �ַ�������βΪ0���ַ���
struct struct_name Ƕ��һ���ṹ��
struct struct_name[5] Ƕ��һ���ṹ�����飬���鳤����СΪ1
dummy[5]    ��ȡ5���ֽں���
dummy[a]    �䳤��������ʧ�ֽ�����[a]��������
```

2)�������͵Ĵ�˺�С�ˣ�
ֻ��uint8_t,uint16_t,uint32_t,uint64_t���ֻ�������֧�ִ�С��
С�ˣ�uint8_t,uint16_t,uint32_t,uint64_t
��ˣ�UINT8_T,UINT16_T,UINT32_T,UINT64_T
�䳤����Ͷ������飬Ҳ��֧�ִ�С����UINT16_T[5]

### 2.����ṹ�����

struct struct_name var;
ʹ��struct�ؼ���������һ���ṹ�����
ע�⣺
  1) �ṹ�����ֺͱ������ֲ�������
  2) �ṹ�����֣��������֣����ܺ�shell��������������

### 3.�ṹ���Ա����

1)���ʻ������ͳ�Ա
  ${var[a]}  ${var[b]}  ${var[c]}  ${var[d]}
  var.a = 4 $(var.a)
2)�����������ͳ�Ա(���������䳤����)
  ${var[e[0]]}  ${var[e[1]]}  ${var[e[2]]}  ${var[e[3]]}  ${var[e[4]]}
  var.e[0] = 5
  ��Ա����������������ʣ���һ����Ա��0��ʼ
3)������Ƕ�ṹ��ĳ�Ա
  ${var[n.a]}  ${var[n.b]}
  var[n.a]=4  var[n.b]=5
  var.n.a = 4
4)������Ƕ�Ľṹ������ĳ�Ա
  ${var[o[0].a]}  ${var[o[4].b]}
  var[o[0].a]=1  var[o[4].b]=4
  var.o[0].a = 1

### 4.�ṹ�帳��ֵ

1)�������ʱ����ֵ
  struct struct_name var = { 1 2 3 4 }
2)֮�󸳳�ֵ
  var = { 1 2 3 4 }

### 5.��stdin��ȡ�ṹ��

1)�������ʱֱ�Ӷ�ȡ
  struct struct_name var _in_;
2)���������
  struct_name var _in_;
or
  var _in_;

### 6.�ѽṹ�������stdout

  struct_name var _out_;
or
  var _out_;

### 7.dump�ṹ��

  struct_name var _dump_;
or
  var _dump_;

### 8.�������ļ���һ�㴦������

1)����ṹ��
2)��stdin���ϵĶ�ȡ�ṹ��
3)��shell���﷨���д������Խṹ������������㣬���ˣ���ֵ������
4)�����stdout

### 9.�ڲ�ԭ��

struct�ؼ����Ǹ�shell������struct_name�Ǹ�shell���飬�����ṹ��Ļ������͡�

��struct���������󣬻�����һ����struct_name��������shell������������struct_name���������ʱ���൱���Ա�����Ϊ����������struct_name������

struct_name�����ڲ������$1(������)����Ϊȫ�ֵĹ������飬ͬʱ��ѽṹ������ͣ�ת��Ϊ�������ͺ�����

```
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

�ṹ��ÿ����Ա�����ͣ�ת��Ϊ���ͺ����Ĳ�����
�������ͺ����ᴦ������Ĳ���($2...$N)��ͬʱ�������������������nameΪ����ֵ

��struct_name�������ݲ�ͬ�ı��������ͻ�õ���ͬ�Ĺ������飬����ζ�Ŷ����˲�ͬ�Ľṹ�������
�ṹ���ڵ�Ԫ�س��ֵ�˳���ܻ��ҡ�

### 10.����

��shell�����������ļ������ܻ�Ƚϲ
������ѭ�����������Աʱ�����ܻἱ���½���
��Ҫ�����ܴ��������ô˹���

### 11.δʵ�ֵĹ���

1) ���ܴ���ָ�����ƣ���������ָ�롢�ṹ��ָ�롢ָ������
2) δ���븡�����͡������ŵ�����
3) ��֧�ָ��������͸�ֵ
4) ��ֵ֧�ֵĲ�����

# ������֧��

### 1.������
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
��ʹ�ùؼ���Class����(����ĸ��д)��֮����������֣���������������������

### 2.�����

```cpp
class Human mark
```


ͨ��`class`�ؼ��������������class����ĸСд��

### 3.���Ա����

��Ա������public��private�ؼ��������壬ͬʱָ����Ա�����Ĺ��к�˽�У�
public��Ա�����������ⲿ���ʣ�private��Ա������ֻ���������ڷ��ʡ�
֧�ֵ���������ͬ�ṹ��֧�ֵ����͡�
1)  ͨ���������ʹ��г�Ա
    mark.name = "Nicolas.Mark"
2)  �����ڷ���ȫ����Ա
    Human::name = "Mark"    #����Ա������ֵ
    $(Human::name)          #��ȡ��Ա����

### 4.���Ա����

���Ա�����������ּ���::�����塣����C++�����ⶨ���Ա����һ����
  Human::speak() { echo hello world }  
  Human������::������ָʾ��speak��������()ָ���Ǻ���
1)  Ĭ�����Ա��������public�ģ�������ʹ��private�ؼ��ְѳ�Ա�������
    ˽�����͡�
        private speak   #��speak��Ա�����ı��private
    �ı��˽�����ͺ󣬲���ͨ����������ⲿ���ʡ�
2)  ͨ���������ʹ��г�Ա����
    mark.speak
3)  �����ڷ���˽�к͹��ó�Ա����
    Human::speak
4)  ���Ա�����ڷ��ʹ��к�˽�����Ա����
    Human::name  Human::height

### 5.�๹�캯��

���캯������C++�Ĺ��캯��������������ͬ�ĳ�Ա������ȷ��Ϊ���캯����
    Human::Human() { :; }
1)  ���캯���ڶ��������ʱ���á�
        class Human mark
    �������Ĺ��캯������ʼ��mark����
2)  û�ж��幹�캯�����࣬Ĭ�ϻ��stdin��ȡ���������������������
    ���ṩ��һֱ����ķ�ʽ����������������
    �������Ҫ���ַ�ʽ������Ҫ�ṩ�չ��캯��

### 6.����������

������������C++����������������������ͬ�ĳ�Ա��������~��ȷ��Ϊ����������
    Human::~Human() { :; }
1)  ����������������ͷ�ʱ���á�
        delete mark
    �ͷ�mark���󣬵�����������
2)  û�ж��������������࣬Ĭ�ϻ���മ�л���stdout��
    ���ṩ��һֱ����ķ�ʽ����������������
    �������Ҫ���ַ�ʽ������Ҫ�ṩ���������������߲�delete����

### 7.�����Ķ�̬�������ͷ�

1)  new �ؼ��ֿ��Զ�̬���������ʹ��Object\*�����á�
        Object\* pobj = new Human
    ����pobjָ��������·����Human����
2)  delete �ؼ���ɾ����̬����������
        class Human mark
        delete mark
    deleteֻ���ͷž�̬����Ķ��󡣽ű��ݲ��߱�����Ķ�̬������
3)  delete\* �ؼ���ɾ����̬����������
        delete\* pobj
    �ͷŶ���
4)  ���ʶ����г�Ա�����͹��г�Ա����
        $pobj.name = "Mark"
        $pobj.speak
        $($pobj.speak)

### 8.��������̳�

��������ͨ���������ڼ���:�ؼ�����ָ��������ġ�����C++�������﷨
    : public|private baseClass
1)  ��������
    �������������������������޷����ʻ����˽�г�Ա�����Է���
    ����Ĺ��г�Ա��
    ������ĳ�Ա������Ҳ�޷����ʻ����˽�г�Ա�����Է��ʻ���Ĺ���
    ��Ա��
2)  ˽������
    �������������޷����ʻ���Ĺ��к�˽�г�Ա��
    ������ĳ�Ա���������Է��ʻ���Ĺ��г�Ա�����ܷ��ʻ����˽�г�Ա

### 9.���������

�������������C++����������ء��﷨��

```cpp
Human:++ () { Human::height ++ }
```

���غ�����ͨ��������һ��ð��(:)�������ص������������ġ�
�����������һԪ�ģ���Ԫ�ġ�����һԪ�����������������thisָ�뷽ʽ���ݣ����غ�����ʹHuman::height�����ķ�ʽ�����������ĳ�Ա������
���ڶ�Ԫ���������������Ҳ����thisָ�뷽ʽ���ݣ��Ҳ���������Ժ���������ʽ���ݣ�ͨ��$1�����ʣ�����ʱ�Ҳ��������ֻ��ͨ�����еĳ�Ա��

```cpp
    Human:+= () {
        if $1 instanceof Human ; then
            Human::height += $($1.height)
        fi
    }
```

?    $1���õ����Ҳ��������$1.height��û�취����˽�г�Ա��


֧�����ص��������
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

### 10.RTTI

1)  instanceof ��������ʶ������Ƿ������ʵ��

```bash
 if mark instanceof Human ; then
     echo yes
 fi
```

2)  typeof �ؼ����ܹ���ö������
    typeof mark  => class Human

### 11.���л�

1)  readin �ؼ��ִ�stdin��ȡ���ݲ���ʼ�����Ա����
2)  putout �ؼ��ִ�stdout���л������ĳ�Ա����
3)  ����������ֻ�������Ա�����ڵ��á�

### 12.��������ƴ������

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
��stdin��ȡBackup�ṹ���ж�idx��Ϊ0��stdout�����

### 13.����Ƕ��ṹ��

����Ƕ��ṹ��ͽṹ����Ƕ��ṹ����һ���ĵ��÷�����
��ṹ�岻�߱���Ա���������Ƕ��Ľṹ���ǹ��еģ������ͨ���������ʡ�
���Ƕ��Ľṹ����˽�еģ���ֻ���������ڷ��ʡ�

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
������ͨ��Human::five.eyes�ķ�ʽ�����ʽṹ���Ա
�����ͨ��mark.five.eyes�ķ�ʽ�����ʹ��еĽṹ���Ա

### 14.����Ƕ����

��Ƚϸ��ӣ�Ƕ����������������ϵ�������г�Ա������������Ƕ�������࣬����
Ƕ�����������õķ�ʽ�����ʵġ�

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
����������$(Human::stupid)�ķ�ʽ����Ƕ�����������ã�Ȼ���ٵ���Ƕ�������ķ������Ա������

�����ͨ��$(mark.stupid)�ķ�ʽ����Ƕ�����������ã�Ȼ���ٵ���Ƕ�������ķ������Ա������

### 15.������ǿ

����bash 4.4�����ϵİ汾���Ỻ��class�������Ϣ���ڶ������нű�ʱ�ٶȻ�졣
4.4���µ�bash�汾��ÿ�ζ��������ж�̬���룬�����ٶ��ϻ���һ����ʧ��
�Ƽ�ʹ��bash 4.4�汾��

### 16.δʵ�ֹ���

1)  ��֧��protected��������

# �ߡ�װ����

�﷨��
    @ DECORATOR Func
ʹ��DECORATORװ����������װ��Func����

### 1.װ��������

1)��̬װ����������

```bash
DECORATOR() {
    echo do something
    $@
    echo do something
}
```
װ����������ͨ��$@������ԭ���������ݲ���
2)��̬װ����������

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
��̬װ��������ͨ����װ�����ڲ�����һ���µĺ�����ͨ��\_\_return����������������ɵĺ�����Ϊװ����������������ص�װ������������Ϊ�Ƕ�̬���ɵġ�

����ͨ��eval "__DECORATOR() { ... }"�����ķ�ʽ��̬���ɣ���ͬһ��װ����װ�β�ͬ�ĺ���ʱ������ÿ�ζ����ز�ͬ��װ����������

### 2.װ����ֻ�ܶ�����Func�������·���

```bash
Func() {
    echo $@
}
@ DECORATOR Func
```

### 3.��ĳ�Ա����Ҳ����ʹ��װ����

```bash
Human::Func() {
    echo $@
}
@ DECORATOR Human::Func
```

### 4.ͨ�õ�shell����Ҳ����ʹ��װ����

```bash
@ DECORATOR ls
ls
@ DECORATOR cd
cd /home
```

### 5.�����ڵ���ĳ����ǰ�ŶԸú�������װ��

���뱣֤װ����ֻ�ܵ���һ�Σ�������ö�Ρ�

### 6.���õ�װ����

@logger  Func   ��ӡ��־
@runtime Func   ��ӡ����ʱ��
@disable Func   ��ֹ��������

