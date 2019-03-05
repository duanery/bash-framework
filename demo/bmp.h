#!/bin/bash

ifndef __BMP_H__
define __BMP_H__

include struct.h

BITMAPFILEHEADER=(
    uint16_t bfType         #2Bytes，必须为"BM"，即0x424D 才是Windows位图文件
    uint32_t bfSize         #4Bytes，整个BMP文件的大小
    uint16_t bfReserved1    #2Bytes，保留，为0
    uint16_t bfReserved2    #2Bytes，保留，为0
    uint32_t bfOffBits      #4Bytes，文件起始位置到图像像素数据的字节偏移量
)
BMP_INFOHEADER=(
    uint32_t biSize         #4Bytes，INFOHEADER结构体大小，存在其他版本I NFOHEADER，用作区分
    uint32_t biWidth        #4Bytes，图像宽度（以像素为单位）
    uint32_t biHeight       #4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
    uint16_t biPlanes       #2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
    uint16_t biBitCount     #2Bytes，图像像素位数
    uint32_t biCompression  #4Bytes，0：不压缩，1：RLE8，2：RLE4
    uint32_t biSizeImage    #4Bytes，4字节对齐的图像数据大小
    uint32_t biXPelsPerMeter #4 Bytes，用象素/米表示的水平分辨率
    uint32_t biYPelsPerMeter #4 Bytes，用象素/米表示的垂直分辨率
    uint32_t biClrUsed       #4 Bytes，实际使用的调色板索引数，0：使用所有的调色板索引
    uint32_t biClrImportant  #4 Bytes，重要的调色板索引数，0：所有的调色板索引都重要
)
struct BITMAPFILEHEADER;
struct BMP_INFOHEADER;

endif
