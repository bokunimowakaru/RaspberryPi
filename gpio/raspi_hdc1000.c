/*******************************************************************************
Raspberry Pi用 I2C 温湿度センサ TI社 HDC1000 raspi_hdc1000

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する
Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

I2C接続の温湿度センサの値を読み取る
TI社 HDC1000

                                        Copyright (c) 2014-2017 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

// #include <Wire.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
byte i2c_address=0x40;				// HDC1000 の I2C アドレス 

uint16_t _getReg(byte data){
	byte rx[2];
    i2c_write(i2c_address,&data,1);	// 書込みの実行
    delay(10);						// 6.5ms以上
    i2c_read(i2c_address,rx,2);		// 読み出し
    return (((uint16_t)rx[0])<<8)|((uint16_t)rx[1]);
}

float getTemp(){
    uint16_t ret;
    ret = _getReg(0x00);			// 温度レジスタ 00
    if(ret<0) return -999.;
    return (float)ret / 65536. * 165. - 40.;
}

float getHum(){
    uint16_t ret;
    ret = _getReg(0x01);			// 湿度レジスタ 01
    if(ret<0) return -999.;
    return (float)ret / 65536. * 100.;
}

int main(int argc,char **argv){
    byte config[3];

    if( argc >= 2 ) i2c_address=(byte)strtol(argv[1],NULL,16);
    if(i2c_address>=0x80) i2c_address>>=1;
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s [i2c_address]\n",argv[0]);
        return -1;
    }

    i2c_init();
	delay(18);							// 15ms以上
    config[0]=0x02;						// 設定レジスタ 02
    config[1]=0x00;
    config[2]=0x00;
    i2c_write(i2c_address,config,3);    // 書込みの実行
    delay(20);

    printf("%3.2f ",getTemp());
    printf("%4.2f\n",getHum());
    
    i2c_close();
    return 0;
}
