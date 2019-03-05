#!/bin/bash
. $(dirname $0)/../include.h
include core.h

include demo/bmp.h

exec <$1
BITMAPFILEHEADER filehdr _in_
BMP_INFOHEADER infohdr _in_

filehdr _dump_
infohdr _dump_
