#!/bin/bash

ifndef __CLASS_H__
define __CLASS_H__

include struct.h

alias Class='function'

class_compiled() {
    #$1 must be associative arrays
    #[ -v $1 ] && [ "$(type -t $1)" == "function" ]
    declare -p $1 &>/dev/null && [ "$(type -t $1)" == "function" ]
}
is_class() {
    class_compiled $1
}

#public struct/class type name
#public type name
#public func => unused
public() {
    [ $# -eq 3 ] && {
        declare strcls=$1
        declare _strcls="$1 "
        shift
    }
    if [ $# -eq 2 ] ; then
        declare type=$1 name=$2
        eval $class[{$name}]="'$_strcls$type'"
        eval $class[[public]]+="' $name'"
        eval $class[[order]]+="' $name'"
        [ "$strcls" == "struct" ] && eval $class[[$strcls]]+="' $name'"
        [ "$strcls" == "class"  ] && eval $class[[delete*]]+="' $name'"
    fi
}
#private type name
#private func
private() {
    [ $# -eq 3 ] && {
        declare strcls=$1
        declare _strcls="$1 "
        shift
    }
    if [ $# -eq 2 ] ; then
        declare type=$1 name=$2
        eval $class[{$name}]="'$_strcls$type'"
        eval $class[[private]]+="' $name'"
        eval $class[[order]]+="' $name'"
        [ "$strcls" == "struct" ] && eval $class[[$strcls]]+="' $name'"
        [ "$strcls" == "class"  ] && eval $class[[delete*]]+="' $name'"
    else
        declare func=$1
        eval $class[\($func\)]=private
    fi
}

#expand `:' command (Do nothing beyond expanding arguments and performing redirections.)
#derive
#: public|private parent_class
:() {
    if [ $# -ne 2 ] ; then
        return 0
    fi
    declare inherit=$1 parent=$2
    class $parent #compile parent class
    eval $class[[inherit]]=$inherit
    eval $class[[parent]]=$parent
}

#=============================================================
# class define
# prefix:
#   __class=1       Generating class:: functions
#   __private=1     Don't generating object. functions
#   __no_objfunc=1  Don't generating $object functions
#
#class Class_name object
class() {
    if class_compiled $1 ; then
        "$@"
        return 0
    fi
    __import_cache $1 &&
        class_compiled $1 && {
            "$@"
            return 0
        }
    
    declare class=$1
    declare -Ag $class
    #get filename
    shopt -s extdebug
    declare info=($(declare -F $class))
    shopt -u extdebug
    eval $class[[filename]]=${info[2]}
    unset -v info
    #call $class
    $class
    unset -f $class
    eval $class[[class]]=$class

    #==================================
    # Generating struct array
    #
    declare name
    eval declare -g __struct_${class}_public='()'
    for name in $(eval echo \${$class[[public]]})
    do
        eval __struct_${class}_public+=\(\${$class[{$name}]} $name\)
    done
    eval declare -g __struct_${class}_private='()'
    for name in $(eval echo \${$class[[private]]})
    do
        eval __struct_${class}_private+=\(\${$class[{$name}]} $name\)
    done
    eval declare -g __struct_${class}_order='()'
    for name in $(eval echo \${$class[[order]]})
    do
        eval __struct_${class}_order+=\(\${$class[{$name}]} $name\)
    done
    
    #==================================
    # Generating class function
    #
    declare func="{
            if [ \$# -eq 0 ] ; then
                return 0
            fi
        "
    #==================================
    # Inheritance relationship
    #
    if eval [ -n \"\${$class[[parent]]}\" ] ; then
        eval declare parent=\${$class[[parent]]}
        eval declare inherit=\${$class[[inherit]]}
        if [ $inherit == private ] ; then
            declare prefix="__private=1"
        fi
        func+="
            $prefix class $parent \$1
        "
        unset -v parent inherit prefix
    fi
    func+="
            __no_objfunc=1 struct __struct_${class}_public \$1
            __private=1 __no_objfunc=1 struct __struct_${class}_private \$1
            #if [ ! -v this ] ; then
                declare this=\$1
                shift
                eval \$this[{class}]=$class
        "
    #==================================
    # Operator overloading
    #
    func+="
                eval function \$this \"{
                    declare this=\$this
                    __overload \\\$@
                }\"
            #else
                #__syntax_error 'Class nesting'
                #declare this=\"\$this.\$1\"
                #shift
            #fi
        "
    #==================================
    # default constructor
    #
    if [ "$(type -t $class::$class)" == function ] ; then
        func+="$class::$class"
    else #default constructor
        func+="member _in_"
    fi
    
    #==================================
    # Private inheritance
    #
    func+="
            if [ -v __private ] && [ \$__private -eq 1 ] ; then
                return 0
            fi
        "
    
    #==================================
    # Member function
    #
    for name in $(compgen -A 'function' "$class::")
    do
        name=${name##*:}
        if [ "$name" == "$class" -o "$name" == "~$class" ] ; then
            continue
        fi
        if eval [ \"\${$class[\($name\)]}\" == private ] ; then
            continue
        fi
        func+="
            eval function \$this.$name \"{
                declare this=\$this
                $class::$name \\\$@
            }\"
        "
        eval $class[[public]]+="' $name'"
    done
    func+="}"
    eval function $class "$func"
    unset -v name func
    
    #==================================
    # Generating $class::$member() functions
    #
    __class=1 __no_objfunc=1 struct __struct_${class}_order $class
    if eval [ -n \"\${$class[[parent]]}\" ] ; then
        eval declare parent=\${$class[[parent]]}
        eval declare inherit=\${$class[[inherit]]}
        declare name __struct
        eval __struct=\${$parent[[struct]]}
        for name in $(eval echo \${$parent[[public]]})
        do
            if [[ "$__struct" =~ "$name" ]] ; then
                for name in $(compgen -A 'function' "$parent::$name.")
                do
                    name=${name##*:}
                    eval function $class::$name "{
                        $parent::$name \$@
                    }"
                    #eval function $class::$name "$(declare -f $parent::$name | tail -n +2)"
                done
                continue
            fi
            if [ "$(type -t $class::$name)" != function ] ; then
                eval function $class::$name "{
                        $parent::$name \$@
                    }"
                #eval function $class::$name "$(declare -f $parent::$name | tail -n +2)"
            fi
        done
        if [ $inherit == public ] ; then
            eval $class[[public]]+=\"\${$parent[[public]]}\"
        fi
        unset -v parent inherit name
    fi
    __cache_to_file
    "$@"
}

#=============================================================
# cache to file
#
__cache_to_file() {
    if [ ${BASH_VERSINFO[0]} -lt 4 ] || [ ${BASH_VERSINFO[1]} -lt 4 ] ; then
        return
    fi
    eval declare filename=\${$class[[filename]]}
    declare dir=$(dirname $filename)
    declare file=$(basename $filename)
    declare path=$dir/cache/$file.cache
    
    declare def=$(declare -p $class)
    echo ${def/declare -/declare -g} >> $path
    declare -f $class >> $path
    #declare -f $(compgen -A 'function' "$class::") >> $path
    declare -f | awk -v class=$class '
        BEGIN { re = "^" class "::.* ()" }
        $0 ~ re { f=1; print; next }
        f==1 && $0 ~ "^{" { fb++; print; next }
        f==1 && $0 ~ "^}" { fb--;if(fb==0)f=0; print; next }
        fb>=1
    ' >> $path
    struct __struct_${class}_public;
    struct __struct_${class}_private;
    struct __struct_${class}_order;
    declare -f __struct_${class}_public >> $path
    declare -f __struct_${class}_private >> $path
    declare -f __struct_${class}_order >> $path
}

__import_cache() {
    if [ ${BASH_VERSINFO[0]} -lt 4 ] || [ ${BASH_VERSINFO[1]} -lt 4 ] ; then
        return 1
    fi
    declare class=$1
    shopt -s extdebug
    declare info=($(declare -F $class))
    shopt -u extdebug
    declare filename=${info[2]}
    [ -z "$filename" ] && return 1
    declare dir=$(dirname $filename)
    declare file=$(basename $filename)
    declare path=$dir/cache/$file.cache
    
    if [ -f "$path" -a $path -nt $filename ] ; then
        include cache/$file.cache
        return 0
    fi
    [ ! -d cache ] && mkdir -p $dir/cache/
    > $path
    return 1
}

#=============================================================
# traverse class member
# prefix:
#   __recurse=1   Recursion to base class
#
__member() {
    if [ -v __recurse ] && [ $__recurse -eq 1 ] ; then
        if eval [ -n \"\${$class[[parent]]}\" ] ; then
            eval class=\${$class[[parent]]} __member $@
        fi
    fi
    __private=1 __struct_${class}_order "" $@
}
member() {
    if [ ! -v this ] ; then
        return 1
    fi
    eval declare class=\${$this[{class}]}
    __member $@
}

#=============================================================
# new, delete
#
____destructor() {
    declare name
    for name in $(eval echo \${$class[[delete*]]})
    do
        delete* $($class::$name)
    done
    unset -v name
    if [ "$(type -t $class::~$class)" == function ] ; then
        $class::~$class
    fi
    if eval [ -n \"\${$class[[parent]]}\" ] ; then
        eval class=\${$class[[parent]]} ____destructor
    fi
}
__destructor() {
    declare this=$1
    eval declare class=\${$this[{class}]}
    [ -z "$class" ] && return 1
    if [ "$(type -t $class::~$class)" != function ] ; then
        #default destructor
        __recurse=1 member _out_
    fi
    ____destructor
}
Object*() {
    if [ $# -ne 4 ] || [ "$2" != '=' -o "$3" != "new" ] ; then
        __syntax_error "Object* obj = new class"
    fi
    declare obj=$1
    new $4
}
new() {
    if [ ! -v obj ] ; then
        __syntax_error "Object* obj = new class"
    fi
    #temporary unset this, not affect external's this
    declare this __class __private __no_objfunc
    unset -v this __class __private __no_objfunc
    eval declare -g $obj=$(mktemp -u __XXXXXXXXXX)
    eval class $1 \$$obj
}
delete() {
    declare obj
    for obj in $@
    do
        __destructor $obj
        unset -f $(compgen -A 'function' "$obj.")
    done
    unset -f $@
    unset -v $@
}
delete*() {
    declare pobj
    for pobj in $@
    do
        delete ${!pobj}
        unset -v $pobj
    done
}

#=============================================================
# Serialization
#
readin() {
    __recurse=1 member _in_
}
putout() {
    __recurse=1 member _out_
}

#=============================================================
# RTTI run-time type identification
#
typeof() {
    declare this=$1
    if eval [ -n \"\${$this[{class}]}\" ] ; then
        eval echo class \${$this[{class}]}
        return
    fi
    if eval [ -n \"\${$this[{struct}]}\" ] ; then
        eval echo struct \${$this[{struct}]}
        return
    fi
}
#instanceof class
__instanceof() {
    eval declare class=\${$this[{class}]}
    while true
    do
        if [ "$class" == "$1" ] ; then
            return 0
        fi
        if eval [ -n \"\${$class[[parent]]}\" ] ; then
            eval class=\${$class[[parent]]}
        else
            break;
        fi
    done
    return 1
}

#=============================================================
# Operator overloading
#
__overload() {
    if [ $# -eq 0 ] ; then
        return
    fi
    declare operator=$1
    shift
    if [ "$operator" == instanceof ] ; then
        __instanceof $@
        return
    fi
    eval declare class=\${$this[{class}]}
    while true
    do
        if [ "$(type -t $class:$operator)" == function ] ; then
            $class:$operator $@
            return 0
        fi
        if eval [ -n \"\${$class[[parent]]}\" ] ; then
            eval class=\${$class[[parent]]}
        else
            break;
        fi
    done
    __syntax_error "Operator '$operator' cannot be overloaded"
}

#=============================================================
# Decorator, Commonly used decorators
#
#@ dec func
@() {
    local __dec=$1 __func=$2
    local __temp=$(mktemp -u __XXXXXX)
    local __return
    $__dec &>/dev/null
    local __cls; unset -v __cls
    [[ $__func =~ "::" ]] && {
        local __prefix="${__func%::*}::"
        [ -v class ] && local __cls=1
    }
    local __prefix=${__prefix}${__temp}_
    if [ -n "$__return" ] ; then
        eval "${__prefix}$(declare -f $__return)"
        __dec=$__prefix$__return
        [ -v __cls ] && private ${__dec#*::}
    fi
#   Decorator class member function, keep class:: prefix
#   __cache_to_file can also cache original member function.
    if [ "$(type -t $__func)" == "function" ] ; then
        local body=$(declare -f $__func)
        eval "${body/#${__func} ()/$__prefix${__func#*::} ()}"
        [ -v __cls ] && private ${__temp}_${__func#*::}
    else
        __prefix="command "
    fi
    eval "${__func}() {
            $__dec $__prefix${__func#*::} \$@
        }"
}
__logger() {
    local __func=$1
    shift
    echo == call ${FUNCNAME[1]} $@ ==
    $__func $@
    echo == end ${FUNCNAME[1]} ==
}
__runtime() {
    local __stime=$(date +%s.%N)
    $@
    local __etime=$(date +%s.%N)
    echo == ${FUNCNAME[1]} runtime: \
        $(awk -v stime=$__stime -v etime=$__etime 'BEGIN{printf("%fs", etime-stime-0.00046765)}') ==
}
__disable() {
    :
}
@logger() {
    @ __logger $1
}
@runtime() {
    @ __runtime $1
}
@disable() {
    @ __disable $1
}

endif
