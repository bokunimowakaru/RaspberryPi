/*******************************************************************************
Raspberry Pi用 I2C液晶 表示プログラム  raspi_lcd

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続の小型液晶に文字を表示する

オプション
  -i		I2C通信のエラーを無視する
  -f		標準入力から待ち受けを行う（終了しない）
  -rPORT	液晶のリセット信号用GPIOポート番号
  -wWIDTH	液晶の表示桁数8または16
  -yROW		表示行1または2
										Copyright (c) 2014-2023 Wataru KUNINO
										https://bokunimo.net/raspi/
							 			https://bokunimo.net/
********************************************************************************
2022/12/25 raspi-gpio対応検討中【製作中・途中版】
# (参考文献)GPIO用コマンド
#   raspi-gpio help
********************************************************************************
元ファイル：
https://github.com/bokunimowakaru/RaspberryPi/blob/master/gpio/raspi_lcd.c
********************************************************************************
最新ファイル：
https://bokunimo.net/git/raspi_lcd/blob/master/raspi_lcd.c
###################### 【要注意】"raspi_i2c.h"のパスを要確認 ###################
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "raspi_i2c.h"  // ################ 【要注意】パス確認 ############
#include "../libs/soft_i2c.h" // #############       元ファイル版 ############

typedef unsigned char byte;
extern int ERROR_CHECK;				// オプション -i
int LOOP=0;							// オプション -f
int PORT=-1;						// オプション -rPORT
int WIDTH=8;						// オプション -wWIDTH
int ROW=0;							// オプション -yROW
int NOINIT=0;						// オプション -n

int main(int argc,char **argv){
	int num=1; char s[97]; s[0]='\0';
	while(argc >=num+1 && argv[num][0]=='-'){
		if(argv[num][1]=='i') ERROR_CHECK=0;
		if(argv[num][1]=='f') LOOP=1;
		if(argv[num][1]=='n') NOINIT=1;
		if(argv[num][1]=='r'){
			PORT=atoi(&argv[num][2]);
			if( PORT == 0 && argc > num+1 ){
				num++;
				PORT = atoi(argv[num]);
			}
			printf("reset (%d)\n",PORT);
			i2c_hard_reset(PORT);
		}
		if(argv[num][1]=='w'){
			WIDTH=atoi(&argv[num][2]);
			if( WIDTH == 0 && argc > num+1 ){
				num++;
				WIDTH = atoi(argv[num]);
			}
			if( WIDTH != 8 && WIDTH != 16 && WIDTH != 20 ) WIDTH=8;
			printf("LCD Width (%d)\n",WIDTH);
		}
		if(argv[num][1]=='y'){
			ROW=atoi(&argv[num][2]) - 1;
			if( ROW < 0 && argc > num+1 ){
				num++;
				ROW = atoi(argv[num]) - 1;
			}
			if(ROW != 0 && ROW != 1) ROW = 0;
			printf("LCD Y (%d)\n",ROW + 1);
		}
		if(argv[num][1]=='h'){
			printf("Usage:\n");
			printf("  %s [-i] [-f] [-r port] [-w lcd_width] [-y row] [text...]\n",argv[0]);
			printf("  echo text... | %s [-i] [-f] [-r port] [-w lcd_width] [-y row]\n",argv[0]);
			printf("  %s -h\n\n",argv[0]);
			printf("    options:\n");
			printf("      -i      ignore I2C communication errors\n");
			printf("      -rPORT  set GPIO port number of reset LCD pin; number for PORT\n");
			printf("      -wWIDTH set display digits; 8 or 16 for WITDH\n");
			printf("      -yROW   set display row; 1 or 2 for ROW\n");
			printf("      text... display text string on the LCD\n");
			printf("      -n      skip initializing LCD\n");
			printf("      -f      use standard input, continuously\n");
			printf("      -h      display this help on the terminal\n\n");
			printf("    オプション(in Japanese):\n");
			printf("      -i      I2C通信のエラーを無視する\n");
			printf("      -rPORT  液晶のリセット信号用GPIOポート番号\n");
			printf("      -wWIDTH 液晶の表示桁数8または16\n");
			printf("      -yROW   表示行1または2\n");
			printf("      text... 表示したい文字列\n");
			printf("      -n      液晶の初期化を実行しない\n");
			printf("      -f      標準入力から待ち受けを行う（終了しない）\n");
			printf("      -h      本ヘルプの表示\n");
			return 0;
		}
		num++;
	}
	if(argc==num) fgets(s,sizeof(s),stdin);
	else while(num<argc && strlen(s)<94){
		strncat(s,argv[num],95);
		// utf_del_uni(s);
		strncat(s," ",95);
		num++;
	}
	if(strlen(s)==0){
		if(ERROR_CHECK) strncat(s,"ｴﾗｰ ｦ ﾑｼ ｼﾃ ｿｳｼﾝ",95);
		else strncat(s,"ﾎﾞｸﾆﾓﾜｶﾙ Rasp.Pi",95);
	}
	if( !i2c_init() ){
		fprintf(stderr,"I2C ERROR in INIT\n");
		if( ERROR_CHECK ) return 1;
	}
//	if( !i2c_lcd_init() ){
	if( !ROW ){ // 1行目
		if( PORT < 0 && NOINIT ) i2c_lcd_set_xy(WIDTH,2);
		else if( !i2c_lcd_init_xy(WIDTH,2) ){
			fprintf(stderr,"I2C ERROR in LCD_INIT\n");
			if( ERROR_CHECK ) return 2;
		}
		if( !i2c_lcd_print(s) ){
			fprintf(stderr,"I2C ERROR in LCD_PRINT row=1\n");
			if( ERROR_CHECK ) return 3;
		}
	}else{		// 2行目が指定されている時
		if( PORT < 0 || NOINIT )i2c_lcd_set_xy(WIDTH,2);
		else if( !i2c_lcd_init_xy(WIDTH,2) ){
			fprintf(stderr,"I2C ERROR in LCD_INIT\n");
			if( ERROR_CHECK ) return 2;
		}
		if( !i2c_lcd_print2(s) ){
			fprintf(stderr,"I2C ERROR in LCD_PRINT row=2\n");
			if( ERROR_CHECK ) return 3;
		}
	}
	while(LOOP && !feof(stdin) ){
		fgets(s,sizeof(s),stdin);
		if( !i2c_lcd_print(s) ){
			i2c_hard_reset(PORT);
			fprintf(stderr,"I2C ERROR in LOOP mode\n");
		}
		if(strncmp(s,"[EOF]",5)==0) LOOP=0;
	}
	i2c_close();
	return 0;
}
