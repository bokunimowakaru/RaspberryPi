/*******************************************************************************
Raspberry Pi用 I2C 加速度センサ Analog Devices 社 ADXL345

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

・I2C接続の加速度センサの値を読み取る

コンパイル方法
    make または gcc -Wall -O1 raspi_adxl345.c soft_i2c.o -o raspi_adxl345

使い方
    ./raspi_adxl345                     デフォルトで動作
    ./raspi_adxl345 1D                  I2Cアドレスを0x1Dに設定
    ./raspi_adxl345 53                  I2Cアドレスを0x53に設定(SDO=Lのとき)

                                        Copyright (c) 2016-2017 Wataru KUNINO
                                        https://bokunimo.net/raspi/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libs/soft_i2c.h"
//  #define DEBUG

typedef unsigned char byte; 
byte i2c_address=0x1D; // ADXL345 の I2C アドレス SDO=L時は 0x53 へ要変更

int _getReg(byte reg){
    byte data=0x00;
    i2c_write(i2c_address,&reg,1);
	if(!i2c_read(i2c_address,&data,1)) return -1;
	return (int)data;
}

void _setReg(byte reg,byte value){
    byte data[2];
    data[0]=reg; data[1]=value;
    i2c_write(i2c_address,data,2);
}

int _getData(int reg){
    byte data[2];
    int16_t val;
    data[0]=(byte)reg;
    i2c_write(i2c_address,data,1);
	if(!i2c_read(i2c_address,data,2)) return -99999;
	val= data[0];
	val |= data[1]<<8;
    return (int)val;
}

float getAcm(int axis){         // 0:x  1:y  2:z
    int in;
    if(axis<0 || axis>2) axis=0;
    axis = axis * 2 + 0x32;     // アドレス計算
    in = _getData(axis);
    if(in < -32768 ) return -99.;
    return in * 0.004 * 9.80665;
}

int adxlSetup(int range){
    /*
    range :0から3
        0:±2g デフォルト
        1:±4g
        2:±8g
        3:±16g
    */
    
    /* I2Cの開始 */
//  i2c_init();                             // I2Cインタフェースの使用を開始

    /* ID確認 */    
    if(_getReg(0x00) != 0xE5) return -1;    // レジスタ0x00—DEVID（読出し専用）
/*
    Wire.beginTransmission(I2C_adxl);
    Wire.write(0x38);                       // レジスタ0x38—FIFO_CTL（読出し／書込み）
    Wire.write(0x00);                       // FIFO バイパス 使用しない(デフォルト)
    Wire.endTransmission();
*/

    /* 割込み禁止 */
    _setReg(0x2E,0x00);                     // レジスタ0x2E—INT_ENABLE（読出し／書込み）
    _setReg(0x2C,0b00001010);               // レジスタ0x2C—BW_RATE（読出し／書込み）
    //             | |||__|_____ Rate       1111:140uA  1110:90uA 1000:60uA
    //             | ||_________ LOW_POWER
    //             |_|__________ 0

    if(_getReg(0x2D) == 0x00){
        /* 測定レンジ設定 */
        if(range<0||range>3) range=0;
        range |= 0b00101000;
        //         ||||||||_____ Range      -> 変数 rangeから
        //         ||||||_______ Justify
        //         |||||________ FULL_RES   -> 1
        //         ||||_________ 0
        //         |||__________ INT_INVERT -> 1 (割込みピンを負論理に Lアクティブ)
        //         ||___________ SPI
        //         |____________ SELF_TEST
        _setReg(0x31,range);                // DATA_FORMAT（アドレス0x31）

        /* 割込み用のスレッシュレベルを設定 */
        _setReg(0x1D,0x08);                 // レジスタ0x1D—THRESH_TAP（読出し／書込み）
                                            // 0x01 62.5 mg/LSB
                                            // 0x08  500 mg
                                            // 0x10    1 g 重力加速度
                                            // 0xA0   10 g
                                            // 0xFF 約16 g

        /* 割込み用のスレッシュ時間値を設定 */
        _setReg(0x21,0xA0);                 // レジスタ0x1D—THRESH_TAP（読出し／書込み）
                                            // 0x01   625 us/LSB
                                            // 0x10    10 ms
                                            // 0x50    50 ms
                                            // 0xA0   100 ms
                                            // 0xFF 約160 ms

        /* 割込み用のDOUBLE_TAP無視時間値を設定 */
        _setReg(0x22,0x00);                 // レジスタ0x22—Latent（読出し／書込み）
                                            // 0x01 1.25ms/LSB
                                            // 0x10   20 ms
                                            // 0x50  100 ms
                                            // 0xA0  200 ms
                                            // 0xFF 約320 ms

        /* 割込み用のDOUBLE_TAP検出時間値を設定 */
        _setReg(0x23,0x00);                 // レジスタ0x23—Window（読出し／書込み）
                                            // 0x01 1.25ms/LSB
                                            // 0x10   20 ms
                                            // 0x50  100 ms
                                            // 0xA0  200 ms
                                            // 0xFF 約320 ms
        /* 割込み用の軸を設定 */
        _setReg(0x2A,0b00000111);           // レジスタ0x2A—TAP_AXES（読出し／書込み）
        //             |  |||||_____ Z
        //             |  ||||______ Y
        //             |  |||_______ X
        //             |  ||________ Suppress
        //             |__|_________ 0

        _setReg(0x1E,0x00);
        _setReg(0x1F,0x00);
        _setReg(0x20,0x00);
        /* 測定開始 */
        _setReg(0x2D,0b00001000);
        delay(100);
        
        /* 重力値を減算する*/
        /*
        _setReg(0x1E,(byte)((int8_t)(-getAcm(0) / 0.0156 / 9.80665)));
        _setReg(0x1F,(byte)((int8_t)(-getAcm(1) / 0.0156 / 9.80665)));
        _setReg(0x20,(byte)((int8_t)(-getAcm(2) / 0.0156 / 9.80665)));
        */        

        return 1;
    }else{
        /* 測定開始 */
        _setReg(0x2D,0b00001000);           // レジスタ0x2D—POWER_CTL（読出し／書込み）
        //             ||||||||_____ Wakeup 00:Frequency 8(Hz)  11: 1(Hz)
        //             ||||||_______ Sleep
        //             |||||________ Measure
        //             ||||_________ AUTO_SLEEP
        //             |||__________ Link
        //             ||___________ 0
        //             |____________ 0

        return 0;
    }
}

void adxlINT(){
    _setReg(0x2D,0x00);                     // 測定停止
    
    _setReg(0x2C,0b00011000);               // レジスタ0x2C—BW_RATE（読出し／書込み）
    //             | |||__|_____ Rate       通常時      1100:140uA  1010:140uA  1000:60uA
    //             | ||_________ LOW_POWER  LowPower時  1100:90uA   1010:50uA   1000:40uA
    //             |_|__________ 0
    
    while(_getReg(0x30)& 0b11111101 ){      // 割込みフラグの終了待ち Watermarkは除外
        getAcm(0);
        getAcm(1);
        getAcm(2);
    }
    /* 割込み開始 */
    _setReg(0x2E,0b01000000);           // レジスタ0x2E—INT_ENABLE（読出し／書込み）
    //             ||||||||_____ Overrun
    //             |||||||______ Watermark
    //             ||||||_______ FREE_FALL
    //             |||||________ Inactivity
    //             ||||_________ Activity
    //             |||__________ DOUBLE_TAP
    //             ||___________ SINGLE_TAP  -> 1
    //             |____________ DATA_READY
    _setReg(0x2D,0b00001000);           // レジスタ0x2D—POWER_CTL（読出し／書込み）
    //             ||||||||_____ Wakeup 00:Frequency 8(Hz)  11: 1(Hz)
    //             ||||||_______ Sleep
    //             |||||________ Measure
    //             ||||_________ AUTO_SLEEP
    //             |||__________ Link
    //             ||___________ 0
    //             |____________ 0
}

int adxlEnd(){
    _setReg(0x2E,0x00);                     // 割込み禁止
    _setReg(0x2D,0x00);                     // 測定停止
    return 0;
}

/* デバッグ用 主要レジスタ表示 */
void adxlStat(){
  printf("0x00 DEVID       0x%02X\n",_getReg(0x00));
  printf("0x1D THRESH_TAP  0x%02X\n",_getReg(0x1D));
  printf("0x1E OFSX        0x%02X\n",_getReg(0x1E));
  printf("0x1F OFSY        0x%02X\n",_getReg(0x1F));
  printf("0x20 OFSZ        0x%02X\n",_getReg(0x20));
  printf("0x21 DUR         0x%02X\n",_getReg(0x21));
  printf("0x22 Latent      0x%02X\n",_getReg(0x22));
  printf("0x23 Window      0x%02X\n",_getReg(0x23));
  printf("0x2A TAP_AXES    0x%02X\n",_getReg(0x2A));
  printf("0x2D POWER_CTL   0x%02X\n",_getReg(0x2D));
  printf("0x2E INT_ENABLE  0x%02X\n",_getReg(0x2E));
  printf("0x2F INT_MAP     0x%02X\n",_getReg(0x2F));
  printf("0x30 INT_SOURCE  0x%02X\n",_getReg(0x30));
  printf("0x31 DATA_FORMAT 0x%02X\n",_getReg(0x31));
  printf("0x38 FIFO_CTL    0x%02X\n",_getReg(0x38));
  printf("0x39 FIFO_STATUS 0x%02X\n",_getReg(0x39));
}

int main(int argc,char **argv){
    int i,start;
    float acm;
    
    if( argc >= 2 ) i2c_address=(byte)strtol(argv[1],NULL,16);
    if(i2c_address>=0x80) i2c_address>>=1;
    #ifdef DEBUG
        printf("address =0x%02X\n",i2c_address);
    #endif
    
    i2c_init();
    start=adxlSetup(0);                     // 加速度センサの初期化と結果取得
    #ifdef DEBUG
    switch(start){                          // 初期化時の結果に応じた表示を実行
        case 0:  printf("Accem Started\n");        break;
        case 1:  printf("Accem Initialized\n");    break;
        default: printf("Accem ERROR\n");          break;
    }
	#endif
    for(i=0;i<3;i++){
        acm=getAcm(i);                      // 加速度を取得
        printf("%0.1f",acm);				// 結果出力[mV]
        if(i < 2) putchar(' ');          	// 区切り文字(スペース)出力
    }
    putchar('\n');
    adxlEnd();
    i2c_close();
    return start;
}
