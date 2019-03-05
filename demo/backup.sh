#!/bin/bash
. $(dirname $0)/../include.h
Options=(
    "[vdiname]"
    "restore [snapshot] images from a backup provided in STDIN"
    "" test N   "run in test mode"
    f  file Y   "restore data to a sparse file"
)
include core.h
source options.h


if [ "$test" = "Y" -o -n "$file" ] ; then
    :
else
    VDI=$1
    if [ -z "$VDI" ] ; then
        echo "Please specify <vdi>"
        Usage
    fi
fi
#==============================================================
# restore
#
include demo/backup.cls

class backup_hdr hdr;
hdr.ok

class obj_backup backup;
while true
do
    backup.readin
    if backup.end ; then
        break;
    fi
    if [ "$test" = "Y" ] ; then
        backup.test
    elif [ -n "$file" ] ; then
        backup.write_file $file
    else
        backup.write_vdi $VDI
    fi
done
