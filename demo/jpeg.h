#!/bin/bash

ifndef __JPEG_H__
define __JPEG_H__

include struct.h

SEGMENT=(               #名称   字节数     数据     说明
    UINT8_T     sid     #段标识   1         FF      每个新段的开始标识
    UINT8_T     stype   #段类型   1                 类型编码（称作“标记码”）
    UINT16_T    size    #段长度       2
    
)

SOI=(
    UINT8_T     sid     #段标识   1         FF      每个新段的开始标识
    UINT8_T     stype
)

APP0=(                  #名称      字节数     值                   说明
    string jfif         #交换格式      5         4A46494600          “JFIF”的ASCII码
    UINT8_T majorV      #主版本号      1
    UINT8_T minorV      #次版本号      1
    UINT8_T density     #密度单位      1         0＝无单位；1＝点数/英寸；2＝点数/厘米
    UINT16_T x          #X像素密度     2                             水平方向的密度
    UINT16_T y          #Y像素密度     2                             垂直方向的密度
    UINT8_T thumbnailX  #缩略图X像素  1                           缩略图水平像素数目 
    UINT8_T thumbnailY  #缩略图Y像素  1                           缩略图垂直像素数目    
)
components=(
    UINT8_T id
    UINT8_T sampling 
    UINT8_T table
)
SOF0=(                  #名称  字节数 值       说明
    UINT8_T precision   #样本精度 1      8       每个样本位数（大多数软件不支持12和16）
    UINT16_T height     #图像高度
    UINT16_T width      #图像宽度
    UINT8_T comps       #
    struct components[comps] comp
)

struct SEGMENT;
struct SOI;
struct APP0;
struct SOF0;

endif
