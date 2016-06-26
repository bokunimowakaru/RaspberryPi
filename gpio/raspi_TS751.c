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
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 

int i2c_temp(){
	/* 温度センサ STTS751 I2Cアドレス 0x39(Addr=33kΩ) */
	int temp;
	byte data;
	
	data=0x00;
	i2c_write(0x39,&data,1);		// レジスタ 0x00(温度上位桁)を指定
	i2c_read(0x39,&data,1);			// 読み出し
	temp = ((int)((signed char)data))*10;
	data=0x02;
	i2c_write(0x39,&data,1);		// レジスタ 0x00(温度上位桁)を指定
	i2c_read(0x39,&data,1);			// 読み出し
	temp += (((int)data/16)*10)/16;
	char s[32];
	sprintf(s,"Temp = %3d.%01d degC",temp/10,temp%10);
	i2c_debug(s,1);
	return(temp);
}

int main(void){
	i2c_init();
	printf("%d\n",i2c_temp());
	i2c_close();
	return 0;
}
