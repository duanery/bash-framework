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
