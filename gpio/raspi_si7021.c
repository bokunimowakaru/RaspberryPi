/*******************************************************************************
Raspberry Pi用 I2C 温度・湿度センサ SILICON LABS Si7021-A10 raspi_si7021

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する

                                        Copyright (c) 2014-2017 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

// usage: raspi_si7021 [i2c_address]
//                      0x40    

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
byte i2c_address=0x40;

// #define DEBUG

uint8_t _si7021_getId(){
    uint8_t tx[2]={0xFC,0xC9};
    uint8_t rx;
    
    i2c_write(i2c_address,tx,2);     // 書込みの実行
    i2c_read(i2c_address,&rx,1);     // 読み出し
    #ifdef DEBUG
        printf("Device ID   =0x%02X\n",rx);
    #endif
    return rx;
}

uint8_t _si7021_getUserReg(){
    uint8_t tx=0xE7;
    uint8_t rx;
    i2c_write(i2c_address,&tx,1);     // 書込みの実行
    i2c_read(i2c_address,&rx,1);      // 読み出し
    #ifdef DEBUG
        printf("User Reg    =0x%02X\n",rx);
    #endif
    return rx;
}

void _si7021_setUserReg(uint8_t in){
    uint8_t tx[2]={0xE6,in};
    i2c_write(i2c_address,tx,2);     // 書込みの実行
    #ifdef DEBUG
        printf("Wrote User Register\n");
    #endif
}

uint16_t _si7021_getReg(byte reg){
    uint8_t tx=reg;
    uint8_t rx[4];
    i2c_write(i2c_address,&tx,1);     // 書込みの実行
    delay(24);                        // 待ち時間 12ms×2倍
    i2c_read(i2c_address,rx,3);
    #ifdef DEBUG
        printf("rx=%02x %02x %02x\n",rx[0],rx[1],rx[2]);
    #endif
    return ((uint16_t)rx[0])<<8 | (uint16_t)rx[1];
}

int setup(){
    byte id,config;
    
    i2c_init();
    id=_si7021_getId();
    if(id != 0x14 && id != 0x15 && id != 0x0D){
        fprintf(stderr,"unknown device\n");
        printf("-1 -1\n");
        return -1;
    }
    
    config=_si7021_getUserReg();
    if(config != 0x3A){
        _si7021_setUserReg(0x3A);
        config=_si7021_getUserReg();
        if(config != 0x3A){
            fprintf(stderr,"cannot initialized\n");
            printf("-1 -1\n");
            return -1;
        }
    }
    delay(20);
    return 0;
}

float getTemp(){
    return 175.72 * (float)_si7021_getReg(0xF3) / 65536. - 46.85;
}

float getHum(){
    return 125. * (float)_si7021_getReg(0xF5) / 65536. - 6.;
}

int main(int argc,char **argv){
    if( argc == 2 ) i2c_address=(byte)strtol(argv[1],NULL,16);
    if( i2c_address>=0x80 ) i2c_address>>=1;
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s [i2c_address]\n",argv[0]);
        return -1;
    }
    #ifdef DEBUG
        printf("i2c_address =0x%02X\n",i2c_address);
    #endif

    setup();
    printf("%3.2f %4.2f\n",getTemp(),getHum());
    i2c_close();
    return 0;
}
