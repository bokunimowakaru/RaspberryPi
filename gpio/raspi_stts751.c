/*******************************************************************************
Raspberry Pi用 I2C 温度センサ STTS751 raspi_stts751

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する
Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                                        Copyright (c) 2014-2017 Wataru KUNINO
                                        https://bokunimo.net/raspi/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
byte i2c_address=0x39;

int i2c_temp(byte i2c_address){
    /* 温度センサ STTS751 I2Cアドレス 0x39(Addr=33kΩ) */
    int temp;
    byte data;
    byte config[2];
    
    config[0]=0x03;
    config[1]=0b10001100;
            //  ||  ||_________________ Tres 解像度 11:12bit 00:10bit
            //  ||_____________________ 0:RUN 1:STOP
            //  |______________________ 0:EVENTピン使用 1:使用しない
            
    i2c_write(i2c_address,config,2);    // レジスタ 0x03設定
    delay(120);                         // 測定待ち 112ms以上
    data=0x00;
    i2c_write(i2c_address,&data,1);     // レジスタ 0x00(温度上位桁)を指定
    i2c_read(i2c_address,&data,1);      // 読み出し
    temp = ((int)((signed char)data))*100;
    data=0x02;
    i2c_write(i2c_address,&data,1);     // レジスタ 0x02(温度下位桁)を指定
    i2c_read(i2c_address,&data,1);      // 読み出し
    temp += ((int)(data>>4))*100/16;
    config[1]=0b11001100;
    i2c_write(i2c_address,config,2);
    return(temp);
}

int main(int argc,char **argv){
    if( argc == 2 ) i2c_address=(byte)strtol(argv[1],NULL,16);
    if( i2c_address>=0x80 ) i2c_address>>=1;
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s [i2c_address]\n",argv[0]);
        return -1;
    }
    i2c_init();
    printf("%3.2f\n",((double)i2c_temp(i2c_address))/100.);
    i2c_close();
    return 0;
}
