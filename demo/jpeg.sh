#!/bin/bash
. $(dirname $0)/../include.h
include core.h

include demo/jpeg.h

exec <$1

SOI _soi _in_;
while true
do
    SEGMENT seg _in_;
    case ${seg[stype]} in
    224) #APP0
        APP0 app0 _in_;
        app0 _dump_
        if [ ${app0[thumbnailX]} -gt 0 -a ${app0[thumbnailY]} -gt 0 ] ; then
            dd of=/dev/null bs=$((3* ${app0[thumbnailX]} * ${app0[thumbnailY]}))
        fi
        ;;
    192) #SOF0
        SOF0 sof0 _in_;
        sof0 _dump_;
        break;
        ;;
    *)
        dd of=/dev/null bs=$((${seg[size]}-2)) count=1 >/dev/null 2>&1
        ;;
    esac
done
