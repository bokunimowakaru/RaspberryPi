/*******************************************************************************
Raspberry Pi用 I2C 温湿度センサ aosong AM2320

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する
Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                                        Copyright (c) 2014-2016 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
int16_t temp,hum=-1;

int i2c_temp(){
    /* 温湿度センサ AM2320 I2Cアドレス 0xB8(8bit) -> 0x5C(7bit) */
    byte data[8];
    byte config[3];
    
    i2c_write(0x5C,config,0);       // 起動コマンド
    delay(15);                      // 起動待ち
    config[0]=0x03;                 // readコマンド
    config[1]=0x00;                 // 開始アドレス
    config[2]=0x04;                 // データ長
    i2c_write(0x5C,config,3);       // レジスタ 0x03設定
    delay(15);                      // 測定待ち15ms
    i2c_read(0x5C,data,6);          // 読み出し
    temp = (((int16_t)data[4])<<8)+((int16_t)data[5]);
    hum  = (((int16_t)data[2])<<8)+((int16_t)data[3]);
    return (int)temp;
}
int i2c_hum(){
    if(hum==-1) i2c_temp();
    return (int)hum;
}

int main(void){
    i2c_init();
    printf("%3.1f ",((float)i2c_temp())/10.);
    printf("%3.1f\n",((float)i2c_hum())/10.);
    i2c_close();
    return 0;
}
