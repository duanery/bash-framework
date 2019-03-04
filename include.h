#!/bin/bash

if [ -z "$__INCLUDE_H__" ] ; then
declare -gi __INCLUDE_H__=0

__probe__() {
    if [ "${FUNCNAME[1]}" = "main" ] ; then
        echo "can't execute include directly"
        exit 1
    fi
    SRC_DIR=$(cd $(dirname "${BASH_SOURCE[0]}"); pwd)
    
    #`source include' in terminal
    if [ ${#FUNCNAME[@]} -eq 2 ] ; then
        SOURCE_INCLUDE=Y
    fi
}

__probe__
unset -f __probe__

__include() {
    builtin . ${SRC_DIR}/$1
}
__include_argv() {
    declare file=$1
    shift
    builtin . ${SRC_DIR}/$file
}

shopt -q -s expand_aliases

alias include='__include'
alias .='__include_argv '
alias source='source '
alias options.h='${SRC_DIR}/options.h'
alias tbl.h='${SRC_DIR}/tbl.h'

__include ifdef.h

fi
