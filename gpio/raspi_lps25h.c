/*******************************************************************************
Raspberry Pi用 I2C 気圧センサ LPS25H raspi_lps25h

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する
Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                                        Copyright (c) 2014-2016 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

// usage: raspi_lps25h [address]
//                      0x5D    
//                      0x5C    SDO（Pin4 SA0)がLowの時

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
byte address=0x5D;

int ahex2i(char c){
    if(c>='0' && c<='9') return c-'0';
    if(c>='a' && c<='f') return c-'a'+10;
    if(c>='A' && c<='F') return c-'A'+10;
    return -1;
}

int _getReg(byte data){
    i2c_write(address,&data,1);     // 書込みの実行
    delay(10);
    i2c_read(address,&data,1);      // 読み出し
    return (int)data;
}

float getTemp(){
    uint16_t ret,in;
    ret = _getReg(0x2C);            // TEMP_OUT_H
    if(ret<0) return -999.;
    ret <<= 8;
    in = _getReg(0x2B);             // TEMP_OUT_L
    if(in<0) return -999.;
    ret += in;
    ret = ~ret + 1;
    return 42.5 - (float)ret / 480.;
}

float getPress(){
    float ret,in;
    in = (float)_getReg(0x2A);      // Press_OUT_H
    if(in<0) return -999.;
    ret = 256. * in;
    in = (float)_getReg(0x29);      // Press_OUT_L
    if(in<0) return -999.;
    ret += in;
    ret *= 256.;
    in = (float)_getReg(0x28);      // Press_OUT_XL
    if(in<0) return -999.;
    ret += in;
    return ret/4096.;
}

int main(int argc,char **argv){
    byte config[2];
    if( argc == 2 ){
        if( strlen(argv[1]) == 2 ){
            address=(byte)ahex2i(argv[1][0]);
            if(address>15) argc=0;
            else address<<=4;
            address+=(byte)ahex2i(argv[1][1]);
            if(address>=0x80) address>>=1;
        }else argc=0;
    }
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s [i2c_address]\n",argv[0]);
        return -1;
    }

    i2c_init();
    if(_getReg(0x0F) != 0xBD ) return -1;
    config[0]=0x20;                 // CTRL_REG_1
    config[1]=0x80;                 // PD=1 , One Shot Mode
    i2c_write(address,config,2);    // 書込みの実行
    config[0]=0x21;                 // CTRL_REG_2
    config[1]=0x01;                 // One Shot
    i2c_write(address,config,2);    // 書込みの実行
    delay(20);

    printf("%3.2f ",getTemp());
    printf("%4.2f\n",getPress());
    
    config[0]=0x20;                 // CTRL_REG_1
    config[1]=0x00;                 // PD=0
    i2c_write(address,config,2);    // 書込みの実行
    i2c_close();
    return 0;
}
