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

#仅内部使用
__co_unset() {
    unset $1
    unset $1_PID
    unset __COEXIT["$1"]
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
    set -m  # Job control is enabled.
    eval "$cmd" 2>/dev/null
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
    if [ -v $1_PID ]
    then
        eval kill -9 -\${$1_PID} &>/dev/null
        eval wait \${$1_PID} &>/dev/null
    fi
    __co_unset $1
}

#cowait
#  无参数：等待全部coproc执行完
#  有参数：等待指定的coproc执行完
#Example:
#  cowait [-n] [PING...]
cowait() {
    # -n 只等待一个协程完成
    declare -i wait_next=0
    if [ x"$1" = x"-n" ]
    then
        shift
        wait_next=1
        break
    fi
    
    if [ $# -gt 0 ]
    then
        if [ $wait_next -eq 1 ]
        then
            # 1.如果有已经完成的直接返回
            declare pids=""
            for co
            do
                if ! [ -v ${co}_PID ]
                then
                    __co_unset $co
                    return 0
                else
                    eval declare pid=\${${co}_PID}
                    pids="$pids $pid"
                fi
            done
            
            # 2.等待一个coproc结束
            wait -n "$pids"
            
            # 3.判断已经完成的协程
            for co
            do
                if ! [ -v ${co}_PID ]
                then
                    __co_unset $co
                    return 0
                else
                    eval declare pid=\${${co}_PID}
                    if ! kill -0 $pid &>/dev/null
                    then
                        __co_unset $co
                        return 0
                    fi
                fi
            done
        else
            # 等待每一个coproc
            for co
            do
                if [ -v ${co}_PID ]
                then
                    eval wait \${${co}_PID} &>/dev/null
                fi
                __co_unset $co
            done
        fi
    else
        # 等待全部coproc执行完
        if [ $wait_next -eq 1 ]
        then
            cowait -n ${!__COEXIT[@]}
        else
            cowait ${!__COEXIT[@]}
        fi
    fi
}

#cocat
#  获得coproc的输出
#Example:
#  cocat PING
cocat() {
    eval declare r=\${$1[0]}
    cat <&$r
    __co_unset $1
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
#  coread PING [-ers] [-a array] [-d delim] [-i text] [-n nchars] [-N nchars] [-p prompt] [-t timeout] [name ...]
#  read 命令的参数，不能使用[-u fd]参数
coread() {
    eval declare r=\${$1[0]}
    shift
    read "$@" <&$r
}

endif
