/*******************************************************************************
Raspberry Pi用 I2C 基本プログラム  raspi_i2c

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続の小型液晶に文字を表示する
Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                               			Copyright (c) 2014-2016 Wataru KUNINO
                               			http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         			// usleep用
#include <ctype.h>						// isprint用
#include <sys/time.h>					// gettimeofday用
#include "../libs/soft_i2c.h"

typedef unsigned char byte; 
int main(int argc,char **argv){
	byte data[2];
	
    if( argc < 1 || argc > 3 ){
        fprintf(stderr,"usage: %s \n",argv[0]);
        printf("9\n");
        return -1;
    }
    
	/* 液晶のセットアップ */
	i2c_init();			// I2Cインタフェースの初期化
	printf("Init LCD\n");
	data[0]=0x00; data[1]=0x39; i2c_write(0x7C,data,2);	// IS=1
	data[0]=0x00; data[1]=0x11; i2c_write(0x7C,data,2);	// OSC
	data[0]=0x00; data[1]=0x70; i2c_write(0x7C,data,2);	// コントラスト	0
	data[0]=0x00; data[1]=0x56; i2c_write(0x7C,data,2);	// Power/Cont	6
	data[0]=0x00; data[1]=0x6C; i2c_write(0x7C,data,2);	// FollowerCtrl	C
	delay(200);
	data[0]=0x00; data[1]=0x38; i2c_write(0x7C,data,2);	// IS=0
	data[0]=0x00; data[1]=0x0C; i2c_write(0x7C,data,2);	// DisplayON	C
	
	printf("Print LCD\n");
	i2c_lcd_print("ﾎﾞｸﾆﾓﾜｶﾙ Rasp.Pi");
	i2c_close();
	
	return 0;
}
