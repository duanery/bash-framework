#!/bin/bash
. $(dirname $0)/include.h
Options=(
    ""
    "test coproc"
)
include core.h
include coproc.h
source options.h

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


costart PING << "EOF"
    ping 127.0.0.1
EOF

cocat PING
