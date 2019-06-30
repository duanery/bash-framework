#!/bin/bash

ifndef __COPOOL_H__
define __COPOOL_H__

include coproc.h

#__lxU6hx601CS8 co mark fd
# co: coproc的名字
# mark: 命令结束标记
# fd: 命令执行完后，写入co，表示协程空闲
__lxU6hx601CS8() {
    declare cmds=""
    echo $2
    echo $1 >&$3   #标记coproc空闲
    while read line
    do
        if [ x"$line" = x"$2" ]
        then
            eval "$cmds"
            cmds=""
            echo $2
            echo $1 >&$3   #执行完成，标记coproc空闲
        else
            cmds="$cmds
                $line"
        fi
    done
}


#copool
#  创建coproc池
#Example:
#  copool NAME size
copool() {
    declare tmp=$(mktemp -u __XXXXXXXX)
    mkfifo /tmp/$tmp
    exec {fd}<>/tmp/$tmp  #动态分配文件描述符赋值给fd
    unlink /tmp/$tmp
    
    declare -gA $1
    eval $1[size]=$2
    eval $1[temp]=$tmp
    eval $1[fd]=$fd
    eval $1[hook]=:  #默认的hook函数`:'
    for((i=0;i<$2;i++))
    do
        costart $1_$i << EOF
            __lxU6hx601CS8 $i $tmp $fd
EOF
    done
}


#coexecin
#  在coproc池中执行程序
#  在不需要协程的输出结果时，可以直接调用这个函数
#Example:
#  coexecin NAME <<< "cmd"
#  coexecin NAME << "EOF"
#    ...
#  EOF
coexecin() {
    eval declare tmp=\${$1[temp]}
    eval declare fd=\${$1[fd]}
    eval declare hook=\${$1[hook]}
    read co <&$fd   #获取一个空闲coproc，如果没有空闲的会一直等待
    echo execin $1_$co
    while coread $1_$co line 
    do
        if [ x"$line" = x"$tmp" ]
        then
            break
        else
            $hook "$line"   #执行钩子函数
        fi
    done
    cowrite $1_$co << EOF
        $(cat)
        $tmp
EOF
}

#copool_hook
#  在coproc池中注册hook程序
#Example:
#  copool_hook NAME function
copool_hook() {
    eval $1[hook]=$2
}

#copool_flush NAME
__WmpXQ3WIVXu0() {
    declare co=$1 tmp=$2 mark=$3 hook=$4
    declare -i marked=0
    while coread $co line 
    do
        if [ x"$line" = x"$tmp" ]
        then
            if [ $marked -eq 1 ]
            then
                break
            fi
        else
            if [ x"$line" = x"$mark" ]
            then
                marked=1
            else
                $hook "$line"   #执行钩子函数
            fi
        fi
    done
}
#copool_flush
#  刷新coproc池中的协程,用于等待全部协程执行完。
#Example:
#  copool_flush NAME
copool_flush() {
    declare mark=$(mktemp -u __XXXXXXXXXXXXXXXX__)
    eval declare size=\${$1[size]}
    eval declare tmp=\${$1[temp]}
    eval declare hook=\${$1[hook]}
    declare -i N=0
    for((i=0;i<$size;i++))
    do
        cowrite $1_$i << EOF
        echo $mark
        echo $tmp
        $tmp
EOF
    done
    for((i=0;i<$size;i++))
    do
        __WmpXQ3WIVXu0 $1_$i $tmp $mark $hook    #刷新函数
    done
}

#copool_wait NAME id
#  等待coproc池中有一个空闲的协程
#  空闲的协程会赋值为id指定的变量
#Example
#  copool_wait NAME id1
copool_alloc() {
    eval declare fd=\${$1[fd]}
    read $2 <&$fd   #获取一个空闲coproc，如果没有空闲的会一直等待
}

#copool_exec NAME id
#  在id指定的协程上运行命令
#  先使用copool_wait申请空闲的协程id
#Example
#Example:
#  copool_exec NAME id <<< "cmd"
#  copool_exec NAME id << "EOF"
#    ...
#  EOF
copool_exec() {
    eval declare tmp=\${$1[temp]}
    declare co=${!2}
    cowrite $1_$co << EOF
        $(cat)
        $tmp
EOF
}

copool_read() {
    eval declare tmp=\${$1[temp]}
    declare __read="coread $1_${!2}"
    declare arg=${!#}  #取最后一个参数名
    shift 2
    $__read "$@"
    if [ x"${!arg}" = x"$tmp" ]  #判断最后一个参数的值是否是tmp值，如果是，意味着协程输出结束
    then
        eval $arg=""
        return 1
    fi
}

#copool_destory
#  销毁coproc池
#Example:
#  copool_destory NAME
copool_destory() {
    eval declare size=\${$1[size]}
    eval declare fd=\${$1[fd]}
    for((i=0;i<$size;i++))
    do
        cokill $1_$i
    done
    eval exec "$fd>&-"  #关闭文件描述符
    unset $1
}

endif
