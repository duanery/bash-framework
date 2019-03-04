#!/bin/bash

#Disable filename expansion (globbing) ‘*’, ‘?’, and ‘[’.
set -f

#扩充shell的加减乘除，自增自减运算
# var + 5, var ++, var *= 5
#
# The following list of operators is grouped into levels 
# of equal-precedence operators.  The levels are listed
# in order of decreasing precedence.
#
#    id++, id--         variable post-increment, post-decrement
#    ++id, --id         variable pre-increment, pre-decrement
#    -, +               unary minus, plus
#    neg(!), compl(~)   logical and bitwise negation
#    **                 exponentiation
#    *, /, %            multiplication, division, remainder
#    +, -               addition, subtraction
#    lshift, rshift     <<, >> left and right bitwise shifts
#    le, ge, lt, gt     <=, >=, <, > comparison
#    ==, !=             equality, inequality
#    and(&)             bitwise AND
#    ^                  bitwise XOR
#    or(|)              bitwise OR
#    land(&&)           logical AND
#    lor(||)            logical OR
#
#    =, *=, /=, %=,
#    +=, -=, lshe(<<=), rshe(>>=),
#    ande(&=), ^=, ore(|=)      assignment


#========================================================
# uint_t
# declare __left=this[a]
# uint_t:++ 16

#uint_t:++ bits
uint_t:++ () {
    local -i __bits=$1
    (($__left++))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:-- bits
uint_t:-- () {
    local -i __bits=$1
    (($__left--))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:~+ bits
uint_t:~+ () {
    local -i __bits=$1
    local -i __val=0
    __val=$((+$__left))
    echo $(($__val & (1<<$__bits)-1 ))
}

#uint_t:~- bits
uint_t:~- () {
    local -i __bits=$1
    local -i __val=0
    __val=$((-$__left))
    echo $(($__val & (1<<$__bits)-1 ))
}


#uint_t:! bits
uint_t:! () {
    ((! $__left))
}

#uint_t:~ bits
uint_t:~ () {
    local -i __bits=$1
    local -i __var=0
    __var=$((~$__left))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:** bits right
uint_t:** () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left**$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:* bits right
uint_t:* () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left*$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:/ bits right
uint_t:/ () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left/$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:% bits right
uint_t:% () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left%$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:+ bits right
uint_t:+ () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left+$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:- bits right
uint_t:- () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left-$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:lshift bits right
#uint_t:<<
uint_t:lshift () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left<<$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:rshift bits right
#uint_t:>>
uint_t:rshift () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left>>$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:le bits right
#uint_t:<=
#less-than-or-equal
uint_t:le () {
    local -i __right=$2
    (($__left<=$__right))
}

#uint_t:ge bits right
#uint_t:>=
#greater-than-or-equal
uint_t:ge () {
    local -i __right=$2
    (($__left>=$__right))
}

#uint_t:lt bits right
#uint_t:<
#less-than
uint_t:lt () {
    local -i __right=$2
    (($__left<$__right))
}

#uint_t:gt bits right
#uint_t:>
#greater-than
uint_t:gt () {
    local -i __right=$2
    (($__left>$__right))
}

#uint_t:== bits right
uint_t:== () {
    local -i __right=$2
    (($__left==$__right))
}

#uint_t:!= bits right
uint_t:!= () {
    local -i __right=$2
    (($__left!=$__right))
}

#uint_t:and bits right
#uint_t:&
uint_t:and () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left&$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:^ bits right
uint_t:^ () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left^$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:or bits right
#uint_t:|
uint_t:or () {
    local -i __bits=$1
    local -i __right=$2
    local -i __var=0
    __var=$(($__left|$__right))
    echo $(($__var & (1<<$__bits)-1 ))
}

#uint_t:land bits right
#uint_t:&&
#logical AND
uint_t:land () {
    local -i __right=$2
    (($__left&&$__right))
}

#uint_t:lor bits right
#uint_t:||
#logical OR
uint_t:lor () {
    local -i __right=$2
    (($__left||$__right))
}

#uint_t:= bits right
uint_t:= () {
    local -i __bits=$1
    local -i __right=$2
    (($__left=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:*= bits right
uint_t:*= () {
    local -i __bits=$1
    local -i __right=$2
    (($__left*=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:/= bits right
uint_t:/= () {
    local -i __bits=$1
    local -i __right=$2
    (($__left/=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:%= bits right
uint_t:%= () {
    local -i __bits=$1
    local -i __right=$2
    (($__left%=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:+= bits right
uint_t:+= () {
    local -i __bits=$1
    local -i __right=$2
    (($__left+=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:-= bits right
uint_t:-= () {
    local -i __bits=$1
    local -i __right=$2
    (($__left-=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:lshe bits right
#uint_t:<<=
#lshift
uint_t:lshe () {
    local -i __bits=$1
    local -i __right=$2
    (($__left<<=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:rshe bits right
#uint_t:>>=
#rshift
uint_t:rshe () {
    local -i __bits=$1
    local -i __right=$2
    (($__left>>=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:ande bits right
#uint_t:&=
uint_t:ande () {
    local -i __bits=$1
    local -i __right=$2
    (($__left&=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:^= bits right
uint_t:^= () {
    local -i __bits=$1
    local -i __right=$2
    (($__left^=$__right))
    (($__left &= (1<<$__bits)-1 ))
}

#uint_t:ore bits right
#uint_t:|=
uint_t:ore () {
    local -i __bits=$1
    local -i __right=$2
    (($__left|=$__right))
    (($__left &= (1<<$__bits)-1 ))
}


#========================================================
# string

#string:= right
string:= () {
    eval $__left="'$@'"
}
#string:+= right
string:+= () {
    eval $__left+="'$1'"
}


++ () {
    $1 ++
}
-- () {
    $1 --
}
+ () {
    $1 '~+'
}
- () {
    $1 '~-'
}
neg () {
    $1 !
}
compl () {
    $1 '~'
}
return


uint32_t () {
    declare -ig $1=0
    eval function $1 "{
        if [ \$# -eq 0 ] ; then
            echo \$$1
            return 0
        fi
        declare __left=$1
        uint_t:\$1 32 \$2
    }"
}


#    id++, id--         variable post-increment, post-decrement
#    ++id, --id         variable pre-increment, pre-decrement
#    -, +               unary minus, plus
#    neg(!), compl(~)   logical and bitwise negation
#    **                 exponentiation
#    *, /, %            multiplication, division, remainder
#    +, -               addition, subtraction
#    lshift, rshift     left and right bitwise shifts
#    le, ge, lt, gt     <=, >=, <, > comparison
#    ==, !=             equality, inequality
#    and                bitwise AND
#    ^                  bitwise XOR
#    or                 bitwise OR
#    land               logical AND
#    lor                logical OR
#
#    =, *=, /=, %=,
#    +=, -=, lshe(<<=), rshe(>>=),
#    ande(&=), ^=, ore(|=)      assignment

uint32_t var
var ++
var
var --
var
var + 1
var - 1
if neg var; then
    echo right 
fi
compl var
var = 3
declare -p var
var ** 3
var * 5
var / 2
var % 2
+ var
- var
var lshift 2
var rshift 1
if var le 5 ;then
    echo $var '<=' 5
fi
if var ge 2 ;then
    echo $var '>=' 2
fi
if var lt 5 ;then
    echo $var '<' 5
fi
if var gt 2 ;then
    echo $var '>' 2
fi
if var == 3 ;then
    echo $var '==' 3
fi
if var != 2 ;then
    echo $var '!=' 2
fi
var and 65535
var ^ 65535
var or 65535
if var land 1 ; then
    echo yes
fi
if var lor 0 ; then
    echo yes
fi

var *= 3
var
var += 4
var
var /= 3
var
var %= 5
var
var += 2
var
var -= 1
var
var lshe 2
var
var rshe 2
var
var ande 55
var
var ^= 66
var
var ore 77
var

