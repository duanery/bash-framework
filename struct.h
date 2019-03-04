#!/bin/bash

include operator.h

ifndef __STDINT_H__
define __STDINT_H__

dump_stack() {
    declare s=0
    while caller $s
    do
        let s++
    done
}

array_exists() {
    declare -p "$1" &> /dev/null
}

__instr() {
    $SRC_DIR/inout
}
__inout() {
    $SRC_DIR/inout 0 $@
}
__INOUT() {
    $SRC_DIR/inout 1 $@
}

__syntax_error() {
    echo "Syntax error : $@" >/dev/fd/2
    dump_stack
    exit 1
}

__assign_uint() {
    if [ "$1" = '{' ] ; then
        if [ $((i+1)) -gt $# ] ; then
            shift $#
        else
            shift $((i+1))
        fi
        case $# in
        0) eval $this[$name]=0  ;;
        1) if [ "$1" = '}' ];then
               eval $this[$name]=0
           else
               __syntax_error "expect '}'"
           fi   ;;
        *) eval $this[$name]=$1 ;;
        esac
    fi
}
__assign_string() {
    if [ "$1" = '{' ] ; then
        if [ $((i+1)) -gt $# ] ; then
            shift $#
        else
            shift $((i+1))
        fi
        case $# in
        0) eval $this[$name]=""  ;;
        1) if [ "$1" = '}' ];then
               eval $this[$name]=""
           else
               __syntax_error "expect '}'"
           fi   ;;
        *) eval $this[$name]=$1 ;;
        esac
    fi
}

#__operate type operator x
__operate() {
    declare ref=${FUNCNAME[1]#*::}
    if [ "$ref" == "${FUNCNAME[1]}" ] ; then
        ref=${FUNCNAME[1]#*.}
    else
        ref=$this.$ref
        ref=${ref#*.}
    fi
    if [ $# -eq 1 ] ; then
        eval echo \${$this[$ref]}
        return 0
    fi
    declare type=$1 op=$2
    shift 2
    declare value="$@"
    #if [ "$(type -t $type:$op)" != function ] ; then
    #    __syntax_error "operand '$op' unexpected"
    #fi
    case $type in
    uint8_t | UINT8_T)
        declare __left=$this[$ref]
        uint_t:$op 8 $value
        ;;
    uint16_t|UINT16_T)
        declare __left=$this[$ref]
        uint_t:$op 16 $value
        ;;
    uint32_t|UINT32_T)
        declare __left=$this[$ref]
        uint_t:$op 32 $value
        ;;
    uint64_t|UINT64_T)
        declare __left=$this[$ref]
        uint_t:$op 64 $value
        ;;
    string)
        declare __left=$this[$ref]
        string:$op "$value"
        ;;
    esac
}

#===================================================
# little endian

#__uint_t bytes name i [= { arg1 ... }]
#__uint_t bytes name i _in_
#__uint_t bytes name i _out_
#__uint_t bytes name i _dump_
__uint_t() {
    declare -i bytes=$1
    shift
    if [ ! -v __private ] ; then
        if [ ! -v __class ] ; then
            if [ "$(type -t $this.$1)" != "function" ]; then
                eval "$this.$1 () { declare this=$this; __operate ${FUNCNAME[1]:2} \$@; }"
            fi
        else
            if [ "$(type -t $this::$1)" != "function" ] ; then
                eval "$this::$1 () { __operate ${FUNCNAME[1]:2} \$@; }"
            fi
            return 0
        fi
    fi
    if [ $# -eq 2 ] ; then
        eval $this[$1]=0
        return
    fi
    declare name=$1 i=$2 op=$3
    shift 3
    case $op in
    =)
        __assign_uint $@
        ;;
    _in_)
        eval $this[$name]=$(__inout $bytes)
        ;;
    _out_)
        eval __inout $bytes \${$this[$name]}
        ;;
    _dump_)
        eval echo $this.$name=\${$this[$name]}
        ;;
    _delete_)
        unset -f $this.$name
        ;;
    esac
}
#__uint8_t name i $@
__uint8_t() {
    __uint_t 1 $@
}
#__uint16_t name i $@
__uint16_t() {
    __uint_t 2 $@
}
#__uint32_t name i $@
__uint32_t() {
    __uint_t 4 $@
}
#__uint64_t name i $@
__uint64_t() {
    __uint_t 8 $@
}

#===================================================
# big endian

#__UINT_T bytes name i $@
__UINT_T() {
    declare -i bytes=$1
    shift
    if [ ! -v __private ] ; then
        if [ ! -v __class ] ; then
            if [ "$(type -t $this.$1)" != "function" ]; then
                eval "$this.$1 () { declare this=$this; __operate ${FUNCNAME[1]:2} \$@; }"
            fi
        else
            if [ "$(type -t $this::$1)" != "function" ] ; then
                eval "$this::$1 () { __operate ${FUNCNAME[1]:2} \$@; }"
            fi
            return 0
        fi
    fi
    if [ $# -eq 2 ] ; then
        eval $this[$1]=0
        return
    fi
    declare name=$1 i=$2 op=$3
    shift 3
    case $op in
    =)
        __assign_uint $@
        ;;
    _in_)
        eval $this[$name]=$(__INOUT $bytes)
        ;;
    _out_)
        eval __INOUT $bytes \${$this[$name]}
        ;;
    _dump_)
        eval echo $this.$name=\${$this[$name]}
        ;;
    _delete_)
        unset -f $this.$name
        ;;
    esac
}
#__UINT8_T name i $@
__UINT8_T() {
    __UINT_T 1 $@
}
#__UINT16_T name i $@
__UINT16_T() {
    __UINT_T 2 $@
}
#__UINT32_T name i $@
__UINT32_T() {
    __UINT_T 4 $@
}
#__UINT64_T name i $@
__UINT64_T() {
    __UINT_T 8 $@
}


#===================================================
# little endian

#__uint_t[] uint len name i $@
__uint_t[]() {
    declare uint=$1
    shift
    if [ $# -lt 3 ] ; then
        __syntax_error "array '$1' need length"
    fi
    declare len=$1 name=$2
    shift 2
    #if len is not a number, then is variable length array
    if ! [ "$len" -ge 0 ] 2>/dev/null ; then
        if [ "${name%.*}" = "$name" ] ; then
            eval len=\${$this[$len]}
        else
            eval len=\${$this[${name%.*}.$len]}
        fi
    fi
    declare -i i=0
    for((i=0; i<len; i++))
    do
        __${uint} "$name[$i]" $@
    done
}
#__uint8_t[] len name i $@
__uint8_t[]() {
    __uint_t[] uint8_t $@
}
#__uint16_t[] len name i $@
__uint16_t[]() {
    __uint_t[] uint16_t $@
}
#__uint32_t[] len name i $@
__uint32_t[]() {
    __uint_t[] uint32_t $@
}
#__uint64_t[] len name i $@
__uint64_t[]() {
    __uint_t[] uint64_t $@
}

#===================================================
# big endian

#__UINT8_T[] len name i $@
__UINT8_T[]() {
    __uint_t[] UINT8_T $@
}
#__UINT16_T[] len name i $@
__UINT16_T[]() {
    __uint_t[] UINT16_T $@
}
#__UINT32_T[] len name i $@
__UINT32_T[]() {
    __uint_t[] UINT32_T $@
}
#__UINT64_T[] len name i $@
__UINT64_T[]() {
    __uint_t[] UINT64_T $@
}


#__uint8_t* name i $@
__uint8_t*() {
    eval $this[$1]=TODO
}
#__uint16_t* name i $@
__uint16_t*() {
    eval $this[$1]=TODO
}
#__uint32_t* name i $@
__uint32_t*() {
    eval $this[$1]=TODO
}
#__uint64_t* name i $@
__uint64_t*() {
    eval $this[$1]=TODO
}

#__string name i $@
__string() {
    if [ ! -v __private ] ; then
        if [ ! -v __class ] ; then
            if [ "$(type -t $this.$1)" != "function" ]; then
                eval "$this.$1 () { declare this=$this; __operate string \$@; }"
            fi
        else
            if [ "$(type -t $this::$1)" != "function" ] ; then
                eval "$this::$1 () { __operate string \$@; }"
            fi
            return 0
        fi
    fi
    if [ $# -eq 2 ] ; then
        eval $this[$1]=""
        return
    fi
    declare name=$1 i=$2 op=$3
    shift 3
    case $op in
    =)
        __assign_string $@
        ;;
    _in_)
        eval $this[$name]=$(__instr)
        ;;
    _out_)
        eval printf -- \${$this[$name]}
        printf "\x00"
        ;;
    _dump_)
        eval echo $this.$name=\${$this[$name]}
        ;;
    _delete_)
        unset -f $this.$name
        ;;
    esac
}

#__struct other name i $@
__struct() {
    declare structname=$1 name=$2 i=$3 op=$4
    shift 3
    if [ $# -eq 0 ] || [ "$op" != '=' ] ; then
        __keep_this=1 struct $structname $name. "$@"
        return
    fi
    shift
    if [ "$1" = '{' ] ; then
        if [ $((i+1)) -gt $# ] ; then
            shift $#
        else
            shift $((i+1))
        fi
        case $# in
        0) __keep_this=1 struct $structname $name. ;;
        1) if [ "$1" = '}' ] ; then
               __keep_this=1 struct $structname $name.
           else
               __syntax_error "expect '}'"
           fi   ;;
        *) __keep_this=1 struct $structname $name. = $1 ;;
        esac
    fi
}

#__struct[] other len name i $@
__struct[]() {
    if [ $# -lt 4 ] ; then
        __syntax_error "array '$1' need length"
    fi
    declare structname=$1 len=$2 name=$3
    shift 4
    #if len is not a number, then is variable length array
    if ! [ "$len" -ge 0 ] 2>/dev/null ; then
        if [ "${name%.*}" = "$name" ] ; then
            eval len=\${$this[$len]}
        else
            eval len=\${$this[${name%.*}.$len]}
        fi
    fi
    declare -i i=0
    for((i=0; i<len; i++))
    do
        __keep_this=1 struct $structname $name[$i]. "$@"
    done
}

#__dummy[] len name i $@
__dummy[]() {
    if [ $# -lt 3 ] ; then
        __syntax_error "array '$1' need length"
    fi
    if [ ! -v __private ] ; then
        if [ ! -v __class ] ; then
            if [ "$(type -t $this.$2)" != "function" ]; then
                eval "$this.$2 () { declare this=$this; __operate uint8_t \$@; }"
            fi
        else
            if [ "$(type -t $this::$2)" != "function" ] ; then
                eval "$this::$2 () { __operate uint8_t \$@; }"
            fi
            return 0
        fi
    fi
    declare len=$1 name=$2 i=$3
    shift 3
    if [ $# -eq 0 ] ; then
        eval $this[$name]=0
        return 0
    fi
    #if len is not a number, then is variable length array
    if ! [ "$len" -ge 0 ] 2>/dev/null ; then
        if [ "${name%.*}" = "$name" ] ; then
            eval len=\${$this[$len]}
        else
            eval len=\${$this[${name%.*}.$len]}
        fi
    fi
    declare op=$1
    shift
    case $op in
    =)
        __assign_uint $@
        ;;
    _in_)
        eval $this[$name]=0
        dd of=/dev/null bs=$len count=1 2>/dev/null
        ;;
    _out_)
        declare -i j=0
        declare var=$this[$name]
        declare c="\\x$(printf %02x ${!var})"
        for((j=0; j<len; j++))
        do
            printf "$c"
        done
        ;;
    _dump_)
        eval echo $this.$name[$len]=\${$this[$name]}
        ;;
    _delete_)
        unset -f $this.$name
        ;;
    esac
}

#__struct Class name i $@
__class() {
    if [ ! -v __private ] ; then
        if [ ! -v __class ] ; then
            if [ "$(type -t $this.$2)" != "function" ]; then
                eval "$this.$2 () { declare this=$this; __operate string \$@; }"
            fi
        else
            if [ "$(type -t $this::$2)" != "function" ] ; then
                eval "$this::$2 () { __operate string \$@; }"
            fi
            return 0
        fi
    fi
    
    declare classname=$1 name=$2 i=$3 op=$4
    eval declare pointer=\${$this[$name]}
    if [ -z "$pointer" ] ; then
        Object* ptemp = new $classname </dev/zero
        eval $this[$name]=$ptemp
        pointer=$ptemp
        unset -v ptemp
    fi
    [ $# -eq 3 ] && return 0
    
    case $op in
    =)  ;;
    _in_|_out_|_dump_)
        declare this=$pointer
        unset -v pointer
        member $op
        ;;
    _delete_)
        delete* $pointer
        ;;
    esac
}
#struct compile to function:
#uint8_t     name  =>  __uint8_t name i $@
#uint16_t    name  =>  __uint16_t name i $@
#uint32_t    name  =>  __uint32_t name i $@
#uint64_t    name  =>  __uint64_t name i $@
#uint8_t[5]  name  =>  __uint8_t[] 5 name i $@
#uint16_t[5] name  =>  __uint16_t[] 5 name i $@
#uint32_t[5] name  =>  __uint32_t[] 5 name i $@
#uint64_t[5] name  =>  __uint64_t[] 5 name i $@
#uint8_t[a]  name  =>  __uint8_t[] a name i $@
#uint16_t[b] name  =>  __uint16_t[] b name i $@
#uint32_t[c] name  =>  __uint32_t[] c name i $@
#uint64_t[d] name  =>  __uint64_t[] d name i $@
#uint8_t*    name  =>  __uint8_t* name i $@
#uint16_t*   name  =>  __uint16_t* name i $@
#uint32_t*   name  =>  __uint32_t* name i $@
#uint64_t*   name  =>  __uint64_t* name i $@
#string      name  =>  __string name i $@
#struct other name =>  __struct other name i $@
#struct other[5] name => __struct[] other 5 name i $@
#dummy[5]    name  =>  __dummy[] 5 name i $@
#dummy[a]    name  =>  __dummy[] a name i $@
#
# prefix:
#   __class=1       Generating class:: functions
#   __private=1     Don't generating object. functions
#   __no_objfunc=1  Don't generating $object functions
#   __keep_this=1   Don't unset this
#
#struct struct_name object
struct() {
    if ! declare -p $1 &>/dev/null ; then
        if [ "$(type -t $1)" = "function" ] ; then
            [ -v __keep_this ] && [ $__keep_this -eq 1 ] || {
                declare this; unset -v this
            }
            "$@"
            return 0
        else
            __syntax_error "$1 is undefined"
        fi
    fi
    if [ -v __no_objfunc ] && [ $__no_objfunc -eq 1 ] ; then
        declare embed=""
    else
        declare embed="
            eval \$this[{struct}]=$1
            if [ \$this != $1 ] ; then
                eval function \$this \"{
                    declare this; unset -v this
                    $1 \$this \\\$@
                }\"
            fi"
    fi
    declare func='{
        if [ ! -v this ] ; then
            if [ $# -eq 0 ];then
                return 0
            fi
            declare this=$1
            shift
            #if [ ! -v $this ] ; then
            if ! declare -p $this &>/dev/null ; then
                declare -Ag $this
';   func+="    $embed
            fi
        else
            if [ \$# -gt 0 ] ; then
                declare prefix=\"\$1\"
                shift
            fi
        fi"
        #
        #    $embed
        #fi
    eval declare -i len=\${#$1[@]}
    declare -i p=0
    declare -i i=0
    declare type name strcls
    for((i=0;i<len;i+=2))
    do
        eval type=\${$1[$i]}
        eval name=\${$1[$((i+1))]}
        if [ "$type" != "struct" -a "$type" != "class" ] ; then
            type=${type//]/ }
            type=${type//[/[] }
            func+="
                __${type} \${prefix}$name $((p++)) \"\$@\"
            "
        else
            let i++
            strcls=$type
            eval type=\${$1[$i]}
            eval name=\${$1[$((i+1))]}
            if [ "$type" = "${type//[/[]}" ] ; then
                func+="
                    __${strcls} ${type} \${prefix}$name $((p++)) \"\$@\"
                "
            else
                type=${type//]/}
                type=${type//[/ }
                func+="
                    __${strcls}[] ${type} \${prefix}$name $((p++)) \"\$@\"
                "
            fi
        fi
    done
    func+="
        }"
    eval function $1 "$func"
    unset -v $1 embed func len p i type name strcls
    [ -v __keep_this ] && [ $__keep_this -eq 1 ] || {
        declare this; unset -v this
    }
    "$@"
}

struct_compiled() {
    ! declare -p $1 &>/dev/null && [ "$(type -t $1)" = "function" ]
}

alias cdefine='__define'


endif
