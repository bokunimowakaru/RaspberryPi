/*******************************************************************************
Raspberry Pi用 I2C i2cdetect

デバイスのI2Cアドレスの検索ツールです。
I2Cアドレス8～119（0x00～0x77）の応答を確認し、表示します。

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

-nオプションで最初に発見したI2Cアドレスのみを応答します

                                        Copyright (c) 2014-2023 Wataru KUNINO
                                        https://bokunimo.net/raspi/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 

int main(int argc,char **argv){
    int i, num=0;
    byte ret;
    if(argc >= 2 && argv[1][0]=='-' && argv[1][1]=='n') num=1;
    if(!num){
        printf("I2C Detector by W.Kunino\n");
        printf("   https://goo.gl/Dmvh2z\n\n");
    }

    ret=0;
    while(!ret){
        ret=i2c_init();
        if( ret==0 ){
            delay(100);
            i2c_close();
        }
    }
    for(i=8;i<120;i++){
        ret=i2c_check(i);
        if(num){
            if(ret){
                printf("%02X\n",i);
                return 0;
            }
        }else{
            if(ret) printf("%02X ",i); else printf("-- ");
            if(i%8==7) printf("\n");
        }
    }
    i2c_close();
    return 0;
}
