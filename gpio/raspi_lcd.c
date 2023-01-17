/*******************************************************************************
Raspberry Pi用 I2C液晶 表示プログラム  raspi_lcd for ST7032i

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
********************************************************************************
参考文献
・秋月電子通商 I2C接続小型8文字×2行液晶 AQM0802A-RN-GBW
　https://akizukidenshi.com/download/ds/xiamen/AQM0802.pdf
・秋月電子通商 AE-AQM1602A(KIT)
　https://akizukidenshi.com/download/ds/xiamen/AQM1602_rev2.pdf
・Sitronix ST7032 Dot Matrix LCD Controller/Driver V1.4 2008/08/18 (Datasheet) 
　https://akizukidenshi.com/download/ds/sitronix/st7032.pdf
・Sitronix ST7032 Dot Matrix LCD Controller/Driver V1.3 2007/11/09 (Datasheet) 
　https://akizukidenshi.com/download/ds/sitronix/ST7032-0Dv1_3.pdf
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "raspi_i2c.h"  // ################### 【要注意】パス確認 ############
#include "../libs/soft_i2c.h" // ################       元ファイル版 ############

#define VER "1.01"

typedef unsigned char byte;
extern int ERROR_CHECK;				// オプション -i
extern int SLOW_MODE;				// オプション -s
int LOOP=0;							// オプション -f
int PORT=-1;						// オプション -rPORT
int WIDTH=8;						// オプション -wWIDTH
int ROW=0;							// オプション -yROW
int NOINIT=0;						// オプション -n
int BAR=0;							// オプション -b
int DOT=0;							// オプション -d

const byte font_lv[64]={
	0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,
	0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x10,
	0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x00,0x10,
	0x03,0x13,0x03,0x13,0x03,0x13,0x00,0x10,
	0x10,0x10,0x00,0x10,0x10,0x00,0x10,0x10,
	0x18,0x18,0x18,0x18,0x18,0x18,0x10,0x10,
	0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x10,0x10,
	0x13,0x13,0x03,0x13,0x13,0x03,0x10,0x10
};

int main(int argc,char **argv){
	int num=1, i, y, bar, i22, dispScale = 4;
	// int peak;
	char s[97]; s[0]='\0';
	while(argc >=num+1 && argv[num][0]=='-'){
		if(atoi(&argv[num][1]) > 0) break;
		if(argv[num][1]=='v') {printf("Version %s\n",VER); return 0;}
		if(argv[num][1]=='i') ERROR_CHECK=0;
		if(argv[num][1]=='s') SLOW_MODE=1;
		if(argv[num][1]=='f') LOOP=1;
		if(argv[num][1]=='n') NOINIT=1;
		if(argv[num][1]=='b') BAR=1;
		if(argv[num][1]=='d'){DOT=1; BAR=1;}
		if(argv[num][1]=='r'){
			PORT=atoi(&argv[num][2]);
			if( PORT == 0 && argc > num+1 ){
				num++;
				PORT = atoi(argv[num]);
			}
			printf("reset (%d)\n",PORT);
			i2c_hard_reset(PORT);
		}
		if(argv[num][1]=='q'){
			PORT=atoi(&argv[num][2]);
			if( PORT == 0 && argc > num+1 ){
				num++;
				PORT = atoi(argv[num]);
			}
			printf("quit (%d)\n",PORT);
			i2c_hard_quit(PORT);
			return 0;
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
			printf("Usage: %s (Version %s)\n",argv[0],VER);
			printf("  %s [-i] [-f] [-r port] [-w lcd_width] [-y row] text...\n",argv[0]);
			printf("  %s [-i] [-f] [-r port] [-w lcd_width] [-y row] [-b|-d] value...\n",argv[0]);
			printf("  echo text... | %s [-i] [-f] [-r port] [-w lcd_width] [-y row]\n",argv[0]);
			printf("  %s -h # shows this help\n",argv[0]);
			printf("  %s -q # releases I2C ports\n\n",argv[0]);
			printf("    options:\n");
			printf("      -i      ignore I2C communication errors\n");
			printf("      -s      slowdown I2C communication mode\n");
			printf("      -rPORT  set GPIO port number of reset LCD pin; number for PORT\n");
			printf("      -wWIDTH set display digits; 8 or 16 for WITDH\n");
			printf("      -yROW   set display row; 1 or 2 for ROW\n");
			printf("      -b      display bar graph\n");
			printf("      -d      display dot graph\n");
			printf("      text... display text string on the LCD\n");
			printf("      -n      skip initializing LCD\n");
			printf("      -f      use standard input, continuously\n");
			printf("      -qPORT  restore GPIO port and I2C ports\n");
			printf("      -h      display this help on the terminal\n");
			printf("      -v      show version (for 1.01 and greater)\n");
			printf("\n");
			printf("    オプション(in Japanese):\n");
			printf("      -i      I2C通信のエラーを無視する\n");
			printf("      -rPORT  液晶のリセット信号用GPIOポート番号\n");
			printf("      -wWIDTH 液晶の表示桁数8または16\n");
			printf("      -yROW   表示行1または2\n");
			printf("      -b      レベルメータ表示\n");
			printf("      -d      ドットメータ表示\n");
			printf("      text... 表示したい文字列\n");
			printf("      -n      液晶の初期化を実行しない\n");
			printf("      -f      標準入力から待ち受けを行う（終了しない）\n");
			printf("      -qPORT  使用していたGPIOポートの開放\n");
			printf("      -h      本ヘルプの表示\n");
			printf("      -v      バージョン表示(1.01～)\n");
			return 0;
		}
		num++;
	}
	/* レベルメータ用 ******************************************************* */
	if((BAR > 0 || DOT > 0) && num < argc){
		if( !i2c_init() ){
			fprintf(stderr,"I2C ERROR in INIT\n");
			if( ERROR_CHECK ) return 1;
		}
		if( PORT < 0 && NOINIT ){
			i2c_lcd_set_xy(WIDTH,2);
		}else{
			if( !i2c_lcd_init_xy(WIDTH,2) ){
				fprintf(stderr,"I2C ERROR in LCD_INIT\n");
				if( ERROR_CHECK ) return 2;
			}else printf("LCD init\n");
			// delay(199);
			i=64; //フォント転送バイト数
			if(WIDTH < 16) i=32;
			if( !i2c_lcd_set_fonts(font_lv, i) ){
				fprintf(stderr,"I2C ERROR in LCD_Set Fonts\n");
				if( ERROR_CHECK ) return 4;
			}
			printf("send %d fonts\n",i/8);
			// delay(199);
		}
		for(y = ROW; y < 2; y++){
			bar = (atoi(argv[num]) * WIDTH) / 50 - 1 + DOT;
			if(bar >= 2 * WIDTH) bar = 2 * WIDTH - 1;
			printf("bar=%d\n",bar);
			for(i=0;i<WIDTH;i++){
				i22 = i * 2 + 1;				// セルの右側に相当するレベル値
				if(i == 0){
					if(bar < 0) s[0] = 0x01;
					else if(bar == 0) s[0] = 0x01;
					else if(bar == 1) s[0] = !DOT ? 0x02 : 0x03;
					else s[0] = !DOT ? 0x02 : 0x00;
				}else if(i < bar / 2){			// セル位置がレベル未満の時
					s[i] = !DOT ? 0x02 : 0x00;	// セルの両側を点灯
				}else if(i == bar / 2){	// セル位置がレベル位置の時
					if(i22 == bar) s[i] = !DOT ? 0x02 : 0x03;
					else if (bar>0) s[i] = 0x01;
					else s[i] = 0x00;
				}else{							// 点灯条件に該当しないとき
					s[i] = 0x00;				// 非点灯表示
				}
				if(WIDTH >= 16 && i % dispScale == 0 && s[i] < 0x04){
					s[i] += 0x04;
				}
				// printf("s[%d]=%d\n",i,s[i]);
			}
			if( !i2c_lcd_out(y, (byte*)s) ){
				fprintf(stderr,"I2C ERROR in LCD_OUT row=2\n");
				if( ERROR_CHECK ) return 4;
			}
			num++;
			if(num >= argc){
				// if(y==0)i2c_lcd_out(1, "                ");
				break;
			}
		}
		i2c_close();
		/* 下記はpeak表示ありの場合
		peak = WIDTH * 2;
		for(i=0;i<WIDTH;i++){
			i22 = i * 2 + 1;				// セルの右側に相当するレベル値
			if(i < bar / 2){			// セル位置がレベル未満の時
				s[i] = 0x02;				// セルの両側を点灯
			}else if(i == bar / 2){	// セル位置がレベル位置の時
				if(i22 == bar || i22 == peak){   // セルの右までの時
					s[i] = 0x02;			// セルの両側を点灯
				}else{						// (セルの左までの時)
					if(i==0 && peak == 0){
						s[i] = 0x00;		// レベルなし
					}else{
						s[i] = 0x01;		// セルの左側を点灯
					}
				}
			}else if(i > 0 && i == peak/2){ // ピーク単独表示位置の時
				if(i22 == peak){			// ピーク位置が右側のとき
					s[i] = 0x03;			// セルの右側のみ単独点灯
				}else{						// (ピーク位置が左側の時)
					s[i] = 0x01;			// セルの左側を点灯
				}
			}else{							// 点灯条件に該当しないとき
				s[i] = 0x00;				// 非点灯表示
			}
			if(WIDTH >= 16 && i % dispScale == 0 && s[i] < 0x04){
				s[i] += 0x04;
			}
			printf("s[%d]=%d\n",i,s[i]);
		}
		*/
		return 0;
	}

	/* 通常表示用 *********************************************************** */
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
		} else printf("LCD init\n");
		if( !i2c_lcd_print(s) ){
			fprintf(stderr,"I2C ERROR in LCD_PRINT row=1\n");
			if( ERROR_CHECK ) return 3;
		}
	}else{		// 2行目が指定されている時
		if( PORT < 0 || NOINIT )i2c_lcd_set_xy(WIDTH,2);
		else if( !i2c_lcd_init_xy(WIDTH,2) ){
			fprintf(stderr,"I2C ERROR in LCD_INIT\n");
			if( ERROR_CHECK ) return 2;
		}else printf("LCD init\n");
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
