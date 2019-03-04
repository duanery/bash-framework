#!/bin/bash

if [ -z "$__IFDEF_H__" ] ; then
declare -gi __IFDEF_H__=0

__ifdef() {
    [ -v $1 ]
}
__ifndef() {
    [ ! -v $1 ]
}
__define() {
    declare -gi $1=$2
}

alias ifdef="if __ifdef"
alias ifndef="if __ifndef"
alias define='then __define'
alias endif="fi"

fi
