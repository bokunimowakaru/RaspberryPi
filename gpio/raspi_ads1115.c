/*******************************************************************************
Raspberry Pi用 I2C 16bit ADコンバータ TI ADS1113 ADS1114 ADS1115

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

・I2C接続のADC（ADコンバータ・AD変換器）から電圧値（mV）を取得します。
・AIN0～AIN3まで（デフォルト）に0～2048mVの値を入力してください。
・2048mV（2.048V）を超えないようにしてください。
・アドレス ADDR PIN=GND:0x48, VDD:0x49, SDA:0x4A, SCL:0x4B

コンパイル方法
    make または gcc -Wall -O1 raspi_ads1115.c soft_i2c.o -o raspi_ads1115

使い方
    ./raspi_ads1115                     デフォルトで動作
    ./raspi_ads1115 48                  I2Cアドレスを0x48に設定
    ./raspi_ads1115 48 1                入力数を1に設定(ADS1113 ADS1114)

                                        Copyright (c) 2014-2017 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libs/soft_i2c.h"
//  #define DEBUG

typedef unsigned char byte; 
byte i2c_address=0x48;

int16_t i2c_adc(int ain){
    byte data[2];
    byte config[3];

    config[0]=0x01;                         // configコマンド
    config[1]=0xC5;                         // AIN=null 2V Single
    config[1] |= (byte)((0x3 & ain)<<4);    // AINポート設定
    config[2]=0x83;                         // 128SPS
    i2c_write(i2c_address,config,3);        // レジスタ 0x03設定
    
    delay(8);                               // 測定待ち8ms (1/128 sec.)
    config[0]=0x00;                         // 変換
    i2c_write(i2c_address,config,1);        // レジスタ 0x03設定
    
    delay(8);                               // 測定待ち8ms
    memset(data,0,2);
    i2c_read(i2c_address,data,2);           // 測定の実行
    return (((int16_t)data[0])<<8)|(int16_t)data[1];
}

int main(int argc,char **argv){
    int i,ch=4;
    int16_t adc;
    
    if( argc >= 2 ) i2c_address=(byte)strtol(argv[1],NULL,16);
    if(i2c_address>=0x80) i2c_address>>=1;
    if( argc == 3 ) ch=atoi(argv[2]);
    if( ch<=0 || ch>4 ) ch=4;
    #ifdef DEBUG
        printf("address =0x%02X\n",i2c_address);
        printf("channels=%d\n",ch);
    #endif
    
    i2c_init();
    for(i=0;i<ch;i++){
        adc=i2c_adc(i);                     // AD変換器の値を取得
        if(adc<0)adc=0;                     // GND電位によって負値が出る対策
        printf("%0.1f",((float)(adc))/32767.*2046.);	// 結果出力[mV]
        #ifdef DEBUG
            printf("(%04x)",adc);           // 16進数での表示(DEBUG用)
        #endif
        if(i < ch-1) putchar(' ');          // 区切り文字(スペース)出力
    }
    putchar('\n');
    i2c_close();
    return 0;
}
