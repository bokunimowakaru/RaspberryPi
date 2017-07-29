/*******************************************************************************
Raspberry Pi用 I2C i2cdetect

デバイスのI2Cアドレスの検索ツールです。
I2Cアドレス8～119（0x00～0x77）の応答を確認し、表示します。

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                                        Copyright (c) 2014-2017 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 

int main(void){
    int i;
    byte ret;
    printf("I2C Detector by W.Kunino\n");
    printf("   https://goo.gl/Dmvh2z\n\n");

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
        if(ret) printf("%02X ",i); else printf("-- ");
        if(i%8==7) printf("\n");
    }
    i2c_close();
    return 0;
}
