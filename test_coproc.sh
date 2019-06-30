#!/bin/bash
. $(dirname $0)/include.h
Options=(
    ""
    "test coproc"
)
include core.h
#include coproc.h
include copool.h
source options.h


#=========================
# TEST
costart TEST << "EOF"
    while read line
    do
        echo $line
    done
EOF
cowrite TEST <<< "hello"
coread TEST h
echo $h
cowrite TEST <<< "world!"
coread TEST w
echo -- $w --
cokill TEST


#=========================
# PING
costart PING << "EOF"
    ping -c 4 127.0.0.1 | awk '{print "====", $0;fflush()}'
EOF
costart PINGi << "EOF"
    ping -c 5 127.0.0.1
EOF
costart PING1 << EOF
    ping -c 8 127.0.0.1
EOF
declare -p PING PING_PID
cocat PING
cowait

#=========================
# POOL

copool POOL 4
copool POOL1 5
copool_alloc POOL id
echo $id
copool_exec POOL id <<< "ping -c 1 127.0.0.1"
while copool_read POOL id line
do
    echo $line
done
copool_flush POOL

__dump() {
    echo "$@"
}
copool_hook POOL __dump
#copool_hook POOL1 __dump

for((i=0;i<8;i++))
do
    coexecin POOL << EOF
    ping -c 8 127.0.0.1 | 
        awk '{print "====", \$0;fflush()}'
    for((i=0;i<4;i++))
    do
        echo \$i
    done
EOF
    coexecin POOL1 << EOF
    ping -c 1 192.168.199.9
EOF
done
copool_flush POOL
copool_flush POOL1
copool_destory POOL
copool_destory POOL1

#=========================
# CO IN CO
inco() {
    costart HELP << "EOF"
    ping 127.0.0.1
EOF
}
costart COCO << "EOF"
    inco
    cocat HELP
EOF
cocat COCO
