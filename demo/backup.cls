#!/bin/bash

include class.h

ifndef __BACKUP_CLS__
define __BACKUP_CLS__

__define VDI_BACKUP_FORMAT_VERSION 1
__define VDI_BACKUP_MAGIC 0x11921192
__define UINT32_MAX 0xffffffff

Class backup_hdr {
    private uint32_t version
    private uint32_t magic
    backup_hdr::ok() {
        #if [ $(backup_hdr::version) -ne $VDI_BACKUP_FORMAT_VERSION ] || \
        #    [ $(backup_hdr::magic) -ne $VDI_BACKUP_MAGIC ] ; then
        if backup_hdr::version != $VDI_BACKUP_FORMAT_VERSION || \
            backup_hdr::magic != $VDI_BACKUP_MAGIC ; then
            echo "The backup file is corrupted"
            exit 1
        fi
    }
}

Class obj_backup {
    public uint32_t idx
    public uint32_t offset
    public uint32_t length
    private uint32_t reserved
    
    obj_backup::obj_backup() { :; }
    obj_backup::~obj_backup() { :; }
    obj_backup::readin() {
        readin
    }
    obj_backup::test() {
        local len=$(obj_backup::length)
        printf "%08u %u %u %u\n" $(obj_backup::idx) $(obj_backup::offset) $len $(obj_backup::reserved);
        dd of=/dev/null bs=$len count=1 2>/dev/null
    }
    obj_backup::write_file() {
        local file=$1
        local len=$(obj_backup::length)
        if [ $len -gt 0 ] ; then
            dd of=$file oflag=seek_bytes seek=$(($(obj_backup::idx)*4*1024*1024+$(obj_backup::offset))) \
                bs=$len count=1 conv=notrunc 2>/dev/null
        else
            dd of=$file oflag=seek_bytes seek=$(($(obj_backup::idx)*4*1024*1024+$(obj_backup::offset)-1)) \
                if=/dev/zero bs=1 count=1 conv=notrunc 2>/dev/null
        fi
    }
    obj_backup::write_vdi() {
        local VDI=$1
        local len=$(obj_backup::length)
        if [ $len -gt 0 ] ; then
            dd bs=$len count=1 2>/dev/null |
                dog vdi write $VDI $(($(obj_backup::idx)*4*1024*1024+$(obj_backup::offset))) $len
        fi
    }
    obj_backup::end() {
        [ $(obj_backup::idx) -eq $UINT32_MAX ]
    }
}

endif
