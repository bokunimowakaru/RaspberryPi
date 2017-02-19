/*******************************************************************************
Raspberry Pi用 SPI K熱電対センサ MAXIM社 MAX6675 raspi_max6675

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

SPI接続のK熱電対センサの値を読み取る
MAXIM社 MAX6675

0℃～1024℃の測定に対応
エラー時は-999.9℃を応答

                                        Copyright (c) 2017 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "../libs/soft_spi.h"
// #define DEBUG

typedef unsigned char byte; 

int _getReg(byte ch){
    char s[16];
    int val;
    FILE *pp;
    char wipi[]="/usr/local/bin/gpio gbr 0";
    
    if(ch) wipi[strlen(wipi)-1]='1';
    pp = popen(wipi, "r");
    if(pp==NULL) return -1;
    fgets(s, 15, pp);
    pclose(pp);
    val=atoi(s);
    if(val < 0 && val >= -32768) val += 65536;
    if(val < 0 || val > 65535) return -1;
    return val;
}

float getTemp(int ch){
    int ret;
    ret = _getReg((byte)ch);
    if(ret<0) return -999.9;
    #ifdef DEBUG
        printf("Channel  = %d\n",ch&1);
        printf("b0 state = %d\n",ret&1);
        printf("b1 devID = %d\n",(ret&2)>>1);
        printf("b2 input = %d\n",(ret&4)>>2);
    #endif
    return (float)(ret>>3)/4.;
}

int main(int argc,char **argv){
    int ch=0;

    if( argc >= 2 ) ch=(byte)strtol(argv[1],NULL,16);
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s <ch>\n",argv[0]);
        return -1;
    }

    printf("%4.1f\n",getTemp(ch));
    return 0;
}
