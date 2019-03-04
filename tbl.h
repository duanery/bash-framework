#!/bin/bash

ifndef __TBL_H__
define __TBL_H__

#TblConfigure=(
#   "SQL table name"
#   "Query KEY name"
#   'short option' "permit_set"  "field"
#   ...
#)

if [ -v TblConfigure ] ; then

declare -ag Options
declare -g  TBL="${TblConfigure[0]}"
declare -g  KEY="${TblConfigure[1]}"

Options[0]="<$KEY>"
Options[1]="get/set field in $TBL in SQL"
declare -i o=1
for((i=2; i<${#TblConfigure[@]}; i+=3))
do
    let o++
    Options[$o]=${TblConfigure[$i]}
    let o++
    field=${TblConfigure[$((i+2))]}
    Options[$o]=$field
    let o++
    permit_set=${TblConfigure[$((i+1))]}
    if [ "$permit_set" = "Y" ] ; then
        permit_set=O
    fi
    Options[$o]=$permit_set
    let o++
    if [ "$permit_set" = "O" ] ; then
        Options[$o]="get/set $field field"
    else
        Options[$o]="get $field field"
    fi
done
#-O, --omit-field
let o++ ; Options[$o]=O
let o++ ; Options[$o]=omit-field
let o++ ; Options[$o]=N
let o++ ; Options[$o]="only print result, omit field's name"
#--key
let o++ ; Options[$o]=""
let o++ ; Options[$o]=key
let o++ ; Options[$o]=Y
let o++ ; Options[$o]="specify key field (default: $KEY)"


include psql.h
source options.h

if [ -z "$1" ] ; then
    echo "Please specify ${Options[0]} :"
    Q=$(psql "select $KEY from $TBL")
    echo "$Q"
    exit 1
fi
if [ -v key -a -n "$key" ] ; then
    KEY=$key
fi

#===================================================
#Query 
#
declare -g -i __max_length=0
querys=""
sets=""
for((i=2; i<${#TblConfigure[@]}; i+=3))
do
    permit_set=${TblConfigure[$((i+1))]}
    f=${TblConfigure[$((i+2))]}
    if [ ! -v $f ] ; then continue; fi
    if [ "$permit_set" = "Y" ] && eval [ \"\$$f\" != '-' ] ; then
        eval value=\"\$$f\"
        if [ -z "$sets" ] ; then
            sets="$f='$value'"
        else
            sets="$sets,$f='$value'"
        fi
    else
        if [ -z "$querys" ] ; then
            querys="$f"
        else
            querys="$querys,$f"
        fi
        if [ ${#f} -gt $__max_length ] ; then
            __max_length=${#f}
        fi
    fi
done

Q=$(psql "select $querys from $TBL where $KEY='$1'")

declare -i k=0
for((i=2; i<${#TblConfigure[@]}; i+=3))
do
    permit_set=${TblConfigure[$((i+1))]}
    f=${TblConfigure[$((i+2))]}
    if [ ! -v $f ] ; then continue; fi
    if [ "$permit_set" = "Y" ] && eval [ \"\$$f\" != '-' ] ; then
        continue
    else
        let k++
        if [ "$omit_field" = "Y" ] ; then
            psqlfield "$Q" $k
        else
            printf "%-*s | %s\n" $__max_length $f "$(psqlfield "$Q" $k)"
        fi
    fi
done

#===================================================
#UPDATE 
#
if [ -n "$sets" ] ; then
    psql "UPDATE $TBL SET $sets WHERE $KEY='$1'"
fi


fi

endif
