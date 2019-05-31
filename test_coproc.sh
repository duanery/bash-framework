#!/bin/bash
. $(dirname $0)/include.h
Options=(
    ""
    "test coproc"
)
include core.h
include coproc.h
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
coread TEST
cowrite TEST <<< "world!"
coread TEST
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
