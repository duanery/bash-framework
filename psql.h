#!/bin/bash

ifndef __PSQL_H__
define __PSQL_H__

#------------------------------------------------------------------------
#     PSQL
#------------------------------------------------------------------------
if which psql >/dev/null 2>&1; then

__PSQL=`which psql`
__PSQLPORT=$__srvport_psql
__PSQLIP=$__srvip


#psql -U postgres -t -A -F, -q -c "select * from tbl_vm"
psql() {
    $__PSQL -h $__PSQLIP -p $__PSQLPORT -d cloudbox -U postgres -t -A -F, -q -c "$*"
}
#psqlfield "$Q" 1
psqlfield() {
    awk -F ',' -v K=$2 '{print $K}' <<< "$1"
}
fi


endif
