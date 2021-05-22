/*******************************************************************************
Raspberry Pi用 I2C 温湿度センサ SIIセミコンダクタ社 S-5851A raspi_s5851a

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する
Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

I2C接続の温度センサの値を読み取る
SIIセミコンダクタ社 S-5851A
                                        Copyright (c) 2014-2017 Wataru KUNINO
                                        https://bokunimo.net/raspi/
*******************************************************************************/

// #include <Wire.h> 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
byte i2c_address=0x48;				// S-5851A の I2C アドレス 

int16_t _getReg(byte data){
	byte rx[2];
    i2c_write(i2c_address,&data,1);	// 書込みの実行
    delay(10);						// 6.5ms以上
    i2c_read(i2c_address,rx,2);		// 読み出し
    return (int16_t)(((uint16_t)rx[0])<<8)|((uint16_t)rx[1]);
}

float getTemp(){
    int16_t ret;
    ret = _getReg(0x00);			// 温度レジスタ 00
    return (float)ret / 256.;
}

int main(int argc,char **argv){
    if( argc >= 2 ) i2c_address=(byte)strtol(argv[1],NULL,16);
    if(i2c_address>=0x80) i2c_address>>=1;
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s [i2c_address]\n",argv[0]);
        return -1;
    }

    i2c_init();
    delay(20);

    printf("%3.2f\n",getTemp());
    
    i2c_close();
    return 0;
}
