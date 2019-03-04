#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
//inout 
void usage() {
    printf("inout 0(little)/1(big-Endian) 1/2/4/8 [int]\n"
        "inout -- string\n"
    );
    exit(1);
}

//return 0:little-Endian 1:big-Endian
int sys_endian() 
{
    int a = 1;
    if (*((char *)(&a)) == 1) {
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) 
{
    int bytes;
    int endian=0;
    uint8_t ch[8];
    if(argc == 1) {
        while(read(STDIN_FILENO, ch, 1) == 1) {
            if(*ch == 0) break;
            if(write(STDOUT_FILENO, ch, 1) != 1)
                return 1;
        }
        return 0;
    }
    if(argc == 2) usage();
    
    endian = atoi(argv[1]); //0:little-Endian 1:big-Endian
    bytes = atoi(argv[2]);
    if(argc == 3) {
        if(read(STDIN_FILENO, ch, bytes) == bytes) {
            if(endian != sys_endian()) {
                int i=0;
                uint8_t c;
                for(i=0; i<bytes/2; i++) {
                    c=ch[i];
                    ch[i]=ch[bytes-i-1];
                    ch[bytes-i-1]=c;
                }
            }
            switch(bytes) {
                case 1: printf("%"PRIu8, *(uint8_t*)ch); break;
                case 2: printf("%"PRIu16, *(uint16_t*)ch); break;
                case 4: printf("%"PRIu32, *(uint32_t*)ch); break;
                case 8: printf("%"PRIu64, *(uint64_t*)ch); break;
            }
            return 0;
        }else
            return 1;
    }else if(argc == 4) {
        if(strncmp(argv[3], "0x", 2) == 0 ||
            strncmp(argv[3], "0X", 2) == 0) {
            switch(bytes) {
                case 1: sscanf(argv[3]+2, "%"PRIx8, (uint8_t*)ch); break;
                case 2: sscanf(argv[3]+2, "%"PRIx16, (uint16_t*)ch); break;
                case 4: sscanf(argv[3]+2, "%"PRIx32, (uint32_t*)ch); break;
                case 8: sscanf(argv[3]+2, "%"PRIx64, (uint64_t*)ch); break;
            }
        }else{
            switch(bytes) {
                case 1: sscanf(argv[3], "%"PRIu8, (uint8_t*)ch); break;
                case 2: sscanf(argv[3], "%"PRIu16, (uint16_t*)ch); break;
                case 4: sscanf(argv[3], "%"PRIu32, (uint32_t*)ch); break;
                case 8: sscanf(argv[3], "%"PRIu64, (uint64_t*)ch); break;
            }
        }
        if(endian != sys_endian()) {
            int i=0;
            uint8_t c;
            for(i=0; i<bytes/2; i++) {
                c=ch[i];
                ch[i]=ch[bytes-i-1];
                ch[bytes-i-1]=c;
            }
        }
        if(write(STDOUT_FILENO, ch, bytes) == bytes)
            return 0;
        else
            return 1;
    }
    return 1;
}
