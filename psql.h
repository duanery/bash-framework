#!/bin/bash

ifndef __PSQL_H__
define __PSQL_H__

#------------------------------------------------------------------------
#     PSQL
#------------------------------------------------------------------------
if which psql >/dev/null 2>&1; then
__PATH=$PATH
PATH="${SRC_DIR}:$PATH"

include config.h
__PSQL=`which psql`
__PSQLPORT=$__srvport_pgpool
if [ -f /etc/ark/cluster.ini ] ; then
    __PSQLIP=$(inicfg -f /etc/ark/cluster.ini list DMS | awk -F= -v SIP=$__srvip 'SIP==$2{print $2; nextfile}')
    if [ -z "$__PSQLIP" ] ; then
        __PSQLIP=`inicfg -f /etc/ark/cluster.ini get VIP dmsvip`
    fi
else
    __PSQLIP=$__srvip
fi

PATH=$__PATH
unset __PATH

#psql -p `srvport pgpool` -d cloudbox -U postgres -t -A -F, -q -c "select * from tbl_vm"
psql() {
    $__PSQL -h $__PSQLIP -p $__PSQLPORT -d cloudbox -U postgres -t -A -F, -q -c "$*"
}
#psqlfield "$Q" 1
psqlfield() {
    awk -F ',' -v K=$2 '{print $K}' <<< "$1"
}
fi


endif
