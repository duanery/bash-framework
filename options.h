#!/bin/bash

#
# Options=(
#   'non-option parameter' 
#   ' - program description'
#   'short option' 
#   'long option' 
#   'required argument': Y,N,O
#   'option description'
#   ...
# )
#

ifndef __OPTIONS_H__
define __OPTIONS_H__

if [ -v Options ] ; then

Usage() {
    declare -i name_max_len=16
    printf "Usage: %s" "$(basename $0)"
    for((i=2; i<${#Options[@]}; i+=4))
    do
        ch=${Options[$i]}
        name=${Options[$((i+1))]}
        has_arg=${Options[$((i+2))]}
        if [ -z "$ch" ] ; then ch="-$name"; fi
        case "$has_arg" in
            Y) printf " [-%s %s]" $ch "$name"   ;;
            N) printf " [-%s]" $ch              ;;
            O) printf " [-%s [%s]]" $ch "$name" ;;
        esac
        if [ ${#name} -gt $name_max_len ] ; then
            name_max_len=${#name}
        fi
    done
    if [ -n "${Options[0]}" ] ; then
        printf " [--] %s\n" "${Options[0]}"
    else
        printf "\n"
    fi
    printf "   -   %s\n\n" "${Options[1]}"
    printf "Options:\n"
    for((i=2; i<${#Options[@]}; i+=4))
    do
        ch=${Options[$i]}
        name=${Options[$((i+1))]}
        desc=${Options[$((i+3))]}
        if [ -n "$ch" ] ; then
            printf "  -%s, --%-*s %b\n" "$ch" $name_max_len "$name" "$desc"
        else
            printf "  --%-*s %b\n" $((name_max_len+4)) "$name" "$desc"
        fi
    done
    printf "  -%s, --%-*s %s\n" "h" $name_max_len "help" "display this help and exit"
    exit 0
}

while [ $# -ne 0 ]
do
    case "$1" in
        -h|--help) Usage ;;
        --) shift; break ;;
    esac
    if [ ${#Options[@]} -eq 2 ] ; then
        break
    fi
    if [ "${1:0:1}" != "-" ] ; then
        NonOption="$NonOption '$1'"
        shift
        continue
    fi
    arg=${1#-}
    arg=${arg#-}
    shift
    for((i=2; i<${#Options[@]}; i+=4))
    do
        ch=${Options[$i]}
        name=${Options[$((i+1))]}
        has_arg=${Options[$((i+2))]}
        if [ "$arg" = "$ch" -o "$arg" = "$name" ] ; then
            variable=${name//-/_} # b-1=>b_1
            case "$has_arg" in
                Y)  if [ $# -eq 0 -o "${1:0:1}" = "-" ] ; then
                        printf " -%s, --%s need argument\n" "$ch" "$name"
                        exit 1
                    else
                        eval $variable="'$1'"
                        shift
                    fi
                    ;;
                N)  eval $variable=Y
                    ;;
                O)  if [ $# -eq 0 -o "${1:0:1}" = "-" ] ; then
                        eval $variable="-"
                    else
                        eval $variable="'$1'"
                        shift
                    fi
                    ;;
            esac
        fi
    done
done
if [ ${#Options[@]} -ne 2 ] ; then
    while [ $# -ne 0 ]
    do
        NonOption="$NonOption '$1'"
        shift
    done
    eval set -- $NonOption
    unset NonOption arg ch name has_arg variable
fi

fi

endif
