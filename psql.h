#!/bin/bash

ifndef __PSQL_H__
define __PSQL_H__

#------------------------------------------------------------------------
#     PSQL
#------------------------------------------------------------------------
if which psql >/dev/null 2>&1; then

#psql -t -A -F, -q -c "select * from tbl_vm"
psql() {
    psql -t -A -F, -q -c "$*"
}

#psqlfield "$Q" 1
psqlfield() {
    awk -F ',' -v K=$2 '{print $K}' <<< "$1"
}
fi


endif
