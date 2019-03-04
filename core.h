#!/bin/bash

ifndef __CORE_H__
define __CORE_H__


#------------------------------------------------------------------------
#     SIGINAL Ctl+c
#------------------------------------------------------------------------
if [ "$SOURCE_INCLUDE" != "Y" ] ; then
trap_SIGINT() {
    [ -t 1 ] && echo -n -e "\033[0m\033[?25h"
    trap - INT
    kill -2 $$
}
trap trap_SIGINT SIGINT
fi

#------------------------------------------------------------------------
#     COMMON
#------------------------------------------------------------------------

redecho() {
    echo -n -e "\033[31m$@\033[0m"
}

#--human-readable  print sizes in human readable format (e.g., 1K 234M 2G)
hmreadable() {
    echo $1 | awk '{
        units[0]="KB"
        units[1]="MB"
        units[2]="GB"
        units[3]="TB"
        units[4]="PB"
        units[5]="EB"
        size = $1
        size /= 1024;
        i = 0
        while(i<5 && size>1024) {
            i++
            size /= 1024
        }
        
        if(size > 10)
            printf "%.0lf %s", size, units[i]
        else
            printf "%.1lf %s", size, units[i]
    }' 
}
#hmreadable1 "4194304"
#4.0 MB
hmreadable1() {
    echo $1 | awk '{
        units[0]="MB"
        units[1]="GB"
        units[2]="TB"
        units[3]="PB"
        units[4]="EB"
        size = $1
        size /= 1024 * 1024;
        i = 0
        while(i<4 && size>1024) {
            i++
            size /= 1024
        }
        
        if(size > 10)
            printf "%.0lf %s", size, units[i]
        else
            printf "%.1lf %s", size, units[i]
    }' 
}
#hmtobyte "used >= 5 GB xxxx"
#used >= 5368709120 xxxx
hmtobyte() {
    echo "$1" | awk '{
        units["KB"]=units["K"]=1024
        units["MB"]=units["M"]=1024*1024
        units["GB"]=units["G"]=1024*1024*1024
        units["TB"]=units["T"]=1024*1024*1024*1024
        upper=toupper($0)
        match(upper, "([0-9]{1,}[.]?[0-9]*)[ ]?(K?B?|M?B?|G?B?|T?B?)", bytes)
        if(bytes[2, "length"] == 0) byte=strtonum(bytes[1])
        else byte=units[toupper(bytes[2])]*strtonum(bytes[1])
        printf "%s%d%s\n", substr($0, 1, RSTART-1), byte, substr($0, RSTART+RLENGTH)
    }'
}
#pexist IP process
pexist() {
    declare ip=$1
    declare p=$2
    declare R=$(ssh -p `srvport sshmgt` root@${ip} "for p in \$(pidof -x $p);
        do
            if kill -0 \$p 1>/dev/null 2>&1; then
                echo \$p
            fi
        done" 2>/dev/null)
    if [ -z "$R" ] ; then
        return 1
    else
        return 0
    fi
}

#progress done total
progress() {
    if ! stdout_is_terminal ; then
        return
    fi
    #echo -n -e "\033[?25l"
    awk -v DONE=$1 -v TOTAL=$2 -v COLUMNS=$COLUMNS 'BEGIN{
        flash[0]="|"
        flash[1]="/"
        flash[2]="-"
        flash[3]="\\"
        if(COLUMNS == "") COLUMNS=111
        if(COLUMNS > 111) COLUMNS=111
        barLen=COLUMNS-11
        printf "\r\033[?25l["
        for(i=0; i<barLen; i++) {
            if(TOTAL*(i+1)/barLen <= DONE)
                printf "="
            else if(TOTAL*i/barLen <= DONE &&
                DONE < TOTAL*(i+1)/barLen)
                printf ">"
            else
                printf " "
        }
        printf "][%d%%][%s]", DONE==TOTAL?100:DONE*100/TOTAL, flash[DONE%4]
        if(DONE==TOTAL)
            printf "\n\033[?25h"
        fflush()
    }'
    #echo -n -e "\033[?25h"
}

stdin_is_terminal() {
    [ -t 0 ]
}
stdout_is_terminal() {
    [ -t 1 ]
}
stderr_is_terminal() {
    [ -t 2 ]
}

confirm() {
    read -p "$1" yes
    if [ "$yes" != "yes" ] ; then
        exit 0
    fi
}

endif
