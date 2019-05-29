#!/bin/bash

ifndef __COPROC_H__
define __COPROC_H__

#全局关联数组
#key: coproc的名字
#value: coproc的进程pid
declare -gA __COEXIT

#仅内部使用
#Ctrl + C终止运行时捕获SIGINT信号
#这个是信号处理程序，终止所有coproc
__coexit1() {
    for co in ${!__COEXIT[@]}
    do
        cokill $co
    done
}

#仅内部使用
#Ctrl + C终止运行时捕获SIGINT信号
#附加trap_SIGINT的执行
__coexit() {
    __coexit1
    trap_SIGINT
}

#costart:
#  coproc创建
#  costart "名字" << "EOF"
#      协程命令体，可以包含任意字符，' " $ \
#  EOF
#Example:
#  costart PING << "EOF"
#      ping www.baidu.com > ping.log
#  EOF
costart() {
    declare cmd="coproc $1 { $(cat) ; }"
    set -m
    eval "$cmd"
    set +m
    eval __COEXIT["$1"]=\${$1_PID}
    trap __coexit SIGINT
    trap __coexit1 EXIT
}

#cokill
#  终止coproc
#  cokill "名字"
#Example:
#  cokill PING
cokill() {
    unset __COEXIT["$1"]
    unset $1
    if [ -v $1_PID ]
    then
        eval kill -9 -\${$1_PID} &>/dev/null
        eval wait \${$1_PID} &>/dev/null
    fi
}

#cowait
#  等待coproc执行完
#Example:
#  cowait PING
cowait() {
    eval wait \${$1_PID} &>/dev/null
}

#cocat
#  获得coproc的输出
#Example:
#  cocat PING
cocat() {
    eval declare r=\${$1[0]}
    cat <&$r
}

#cowrite
#  向coproc写入数据
#Example:
#  cowrite PING <<<"somthing"
#  cowrite PING << "EOF"
#      somthing
#  EOF
cowrite() {
    eval declare w=\${$1[1]}
    cat >&$w
}

#coread
#  从coproc读入一行
#Example:
#  coread PING
coread() {
    eval declare r=\${$1[0]}
    read line <&$r
    echo $line
}

endif