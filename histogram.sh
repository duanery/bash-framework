#!/bin/bash

#Usage
#Histogram.sh log2|linear
#从标准输入接收数据，输出直方图
# 一维数据:
# 0  |        test           : count   distribution
# 1  |  _\      0 - 1        : 1     |**********                              |
# 2  |   /      1 - 2        : 1     |**********                              |
# 3  |          2 - 4        : 2     |********************                    |
#
# 二维数据:
# aaa 1   |          aaa
# bbb 2   |                 1 - 1        : 1     |*************                           |
# aaa 4   |   _\            4 - 4        : 1     |*************                           |
# aaa 5   |    /            5 - 5        : 1     |*************                           |
# bbb 3   |          bbb 
# bbb 6   |                 2 - 2        : 1     |*************                           |
#                           3 - 3        : 1     |*************                           |
#                           6 - 6        : 1     |*************                           |

work_dir=$(cd $(dirname $0);pwd)
cd $work_dir
. $(dirname $0)/include.h
Options=(
    "log2|linear"
    "read date from stdin and show histogram"
    "t"  "title"      "Y"  "show title"
    ""   "line1"      "N"  "single line output"
    "m"  "multiply"   "Y"  "multiplication ratio"
    "b"  "base"       "Y"  "linear: base"
    "s"  "step"       "Y"  "linear: step"
    "M"  "marker"     "Y"  "mark for output"
)
line1=0
multiply=1
base=0
step=1
source options.h

mode=$1
awk_var="-v line1=$line1 "

if [ -v title -a -n "$title" ] ; then
    printf "  %-17s : count   distribution\n" "$title"
fi

if [ -v marker -a -n "$marker" ] ; then
    awk_var+="-v marker=$marker "
fi

awk_var+="-v multiply=$multiply "


case $mode in
log2)
    awk $awk_var '
        function print_stars(val, val_max, width) {
            num_stars = int(val * width / val_max)
            for(j=0; j<num_stars; j++)
                printf("*")
            for(; j<width; j++)
                printf(" ")
        }
        function maker_process(mark) {
            if (!line1)
                print mark
            for(slot in count) {
                if (line1)
                    printf("%s ", mark)
                if (twoD)
                    printf("%s ", slot)
                if (line1) {
                    for(i=0; i<=max[slot]; i++) {
                        printf("<%d:%-4d ", lshift(1, i), count[slot][i])
                    }
                    printf("\n")
                } else {
                    printf("\n")
                    printf("%8d - %-8d : %-5d |", 0, 1, count[slot][0])
                    print_stars(count[slot][0], counts[slot], 40)
                    printf("|\n")
                    for(i=1; i<=max[slot]; i++) {
                        printf("%8d - %-8d : %-5d |", lshift(1, i-1), lshift(1, i), count[slot][i])
                        print_stars(count[slot][i], counts[slot], 40)
                        printf("|\n")
                    }
                }
            }
            delete count
            delete counts
            delete max
        }
        BEGIN {
            twoD = 0
        } marker != "" && $0 ~ marker {
            maker_process($0)
            fflush()
            next
        } (NF==1 && twoD==0) || NF==2 {
            slot = 0
            if (NF == 2) {
                twoD = 1
                slot = $1
            }
            num = int($NF*multiply)
            idx = 0
            while(num > 0) {
                idx ++;
                num = rshift(num, 1)
            }
            count[slot][idx]++
            counts[slot]++
            max[slot] = (max[slot] > idx ? max[slot] : idx)
        } END {
            maker_process("")
        }'
    ;;
linear)
    awk_var+="-v base=$base -v step=$step "
    awk $awk_var '
        function print_stars(val, val_max, width) {
            num_stars = int(val * width / val_max)
            for(j=0; j<num_stars; j++)
                printf("*")
            for(; j<width; j++)
                printf(" ")
        }
        function maker_process(mark) {
            if (!line1)
                print mark
            for(slot in count) {
                if (line1)
                    printf("%s ", mark)
                if (twoD)
                    printf("%s ", slot)
                if (line1) {
                    for(i=0; i<=max[slot]; i++) {
                        if (i in count[slot])
                            printf("<%d:%-4d ", base+(i+1)*step, count[slot][i])
                    }
                    printf("\n")
                } else {
                    printf("\n")
                    for(i=0; i<=max[slot]; i++) {
                        if (i in count[slot]) {
                            printf("%8d - %-8d : %-5d |", base+i*step, base+(i+1)*step-1, count[slot][i])
                            print_stars(count[slot][i], counts[slot], 40)
                            printf("|\n")
                        }
                    }
                }
            }
            delete count
            delete counts
            delete max
        }
        BEGIN {
            twoD = 0
        } marker != "" && $0 ~ marker {
            maker_process($0)
            fflush()
            next
        } (NF==1 && twoD==0) || NF==2 {
            slot = 0
            if (NF == 2) {
                twoD = 1
                slot = $1
            }
            num = int($NF*multiply)
            idx = (num - base) / step
            idx = int(idx)

            count[slot][idx]++
            counts[slot]++
            max[slot] = (max[slot] > idx ? max[slot] : idx)
        } END {
            maker_process("")
        }'
    ;;
*)
    Usage
    ;;
esac
