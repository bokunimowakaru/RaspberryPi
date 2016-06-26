/*******************************************************************************
Raspberry Pi用 ソフトウェアI2C ライブラリ  soft_i2c

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

#define PORT_SCL	"/sys/class/gpio/gpio3/value"		// I2C SCLポート
#define PORT_SDA	"/sys/class/gpio/gpio2/value"		// I2C SDAポート
#define INPUT		"in"
#define OUTPUT		"out"
#define LOW			0
#define HIGH		1
#define	I2C_RAMDA	20					// I2C データシンボル長[us]
#define GPIO_RETRY  3       			// GPIO 切換え時のリトライ回数
#define S_NUM       16       			// 文字列の最大長
	#define DEBUG               		// デバッグモード

typedef unsigned char byte; 
FILE *fgpio;
char buf[S_NUM];
struct timeval micros_time;				//time_t micros_time;
int micros_prev,micros_sec=0;

int _micros(){
	int micros;
	gettimeofday(&micros_time, NULL);    // time(&micros_time);
	micros = micros_time.tv_usec;
	if(micros_prev > micros ) micros_sec++;
	micros_prev = micros;
	micros += micros_sec * 1000000;
	return micros;
}

void _micros_0(){
	micros_sec=0;
}

void _delayMicroseconds(int i){
	usleep(i);
}

void delay(int i){
	while(i){
		_delayMicroseconds(1000);
		i--;
	}
}

void pinMode(char *port, char *mode){
	char dir[]="/sys/class/gpio/gpio3/direction";
	         // 0123456789012345678901234567890
	dir[20]=port[20];
    #ifdef DEBUG
    //	printf("pinMode %s %s\n",dir,mode);
    #endif
	fgpio = fopen(dir, "w");
	if( fgpio ){
		fprintf(fgpio,mode);
		fclose(fgpio);
	}
}

byte digitalRead(char *port){
    fgpio = fopen(port, "r");
	if( fgpio ){
	    fgets(buf, S_NUM, fgpio);
	    fclose(fgpio);
	}
    #ifdef DEBUG
    //	printf("digitalRead %s %s\n",port,buf);
    #endif
    return (byte)atoi(buf);
}

void digitalWrite(char *port, int value){
    fgpio = fopen(port, "w");
	if( fgpio ){
	    fprintf(fgpio,"%d\n",value);
	    fclose(fgpio);
	}
    #ifdef DEBUG
    //	printf("digitalWrite %s %d\n",port,value);
    #endif
}

void i2c_debug(const char *s,byte priority){
	printf("[");
	printf("%10d",_micros());
	if(priority>3)	printf("] ERROR:");
	else 			printf("]      :");
	printf(s);
	printf("\n");
}

void i2c_error(const char *s){
	i2c_debug(s,5);
}

void i2c_SCL(byte level){
	if( level ){
		pinMode(PORT_SCL, INPUT);
	}else{
		digitalWrite(PORT_SCL, LOW);
		pinMode(PORT_SCL, OUTPUT);
	}
	_delayMicroseconds(I2C_RAMDA);
}

void i2c_SDA(byte level){
	if( level ){
		pinMode(PORT_SDA, INPUT);
	}else{
		digitalWrite(PORT_SDA, LOW);
		pinMode(PORT_SDA, OUTPUT);
	}
	_delayMicroseconds(I2C_RAMDA);
}

byte i2c_tx(const byte in){
	byte i;
	for(i=0;i<8;i++){
		if( (in>>(7-i))&0x01 ){
				i2c_SDA(1);					// (SDA)	H Imp
		}else	i2c_SDA(0);					// (SDA)	L Out
		/*Clock*/
		i2c_SCL(1);							// (SCL)	H Imp
		i2c_SCL(0);							// (SCL)	L Out
	}
	/* ACK処理 */
	_delayMicroseconds(I2C_RAMDA);
	i2c_SCL(1);								// (SCL)	H Imp
	i2c_SDA(1);								// (SDA)	H Imp
	for(i=50;i>0;i--){
		if( digitalRead(PORT_SDA) == 0 ) break;	// 速やかに確認
		_delayMicroseconds(I2C_RAMDA);
	}
	if(i==0){
		i2c_error("I2C_TX / no ACK");
		return(0);
	}
	return(i);
}

byte i2c_init(void){
	byte i;

	for(i=50;i>0;i--){						// リトライ50回まで
		i2c_SDA(1);							// (SDA)	H Imp
		i2c_SCL(1);							// (SCL)	H Imp
		if( digitalRead(PORT_SCL)==1 &&
			digitalRead(PORT_SDA)==1  ) break;
		delay(1);
	}
	if(i==0) i2c_error("I2C_Init / Locked Lines");
	_delayMicroseconds(I2C_RAMDA*8);
	return(i);
}

byte i2c_start(void){
	if(!i2c_init())return(0);				// SDA,SCL  H Out
	i2c_SDA(0);								// (SDA)	L Out
	_delayMicroseconds(I2C_RAMDA);
	i2c_SCL(0);								// (SCL)	L Out
	return(1);
}

byte i2c_write(byte adr, byte *tx, byte len){
/*
入力：byte adr = I2Cアドレス ／SHT 温度測定時0x01	湿度測定時0x02
入力：byte *tx = 送信データ用ポインタ
入力：byte len = 送信データ長
*/
	byte ret;
	if(len){
		if( !i2c_start() ) return(0);
	}else return(0);
	adr &= 0xFE;							// RW=0 送信モード
	if( i2c_tx(adr)==0 ) return(0);			// アドレス設定

	/* データ送信 */
	for(ret=0;ret<len;ret++){
		i2c_SDA(0);							// (SDA)	L Out
		i2c_SCL(0);							// (SCL)	L Out
		i2c_tx(tx[ret]);
	}
	/* STOP */
	i2c_SDA(0);								// (SDA)	L Out
	i2c_SCL(0);								// (SCL)	L Out
	_delayMicroseconds(I2C_RAMDA);
	i2c_SCL(1);								// (SCL)	H Imp
	i2c_SDA(1);								// (SDA)	H Imp
	return(ret);
}

void i2c_lcd_out(byte y,byte *lcd){
	byte data[2];
	byte i;
	data[0]=0x00;
	if(y==0) data[1]=0x80;
	else{
		data[1]=0xC0;
		y=1;
	}
	i2c_write(0x7C,data,2);
	for(i=0;i<8;i++){
		if(lcd[i]==0x00) break;
		data[0]=0x40;
		data[1]=lcd[i];
		i2c_write(0x7C,data,2);
	}
}

void utf_del_uni(char *s){
	byte i=0;
	byte j=0;
	while(s[i]!='\0'){
		if((byte)s[i]==0xEF){
			if((byte)s[i+1]==0xBE) s[i+2] += 0x40;
			i+=2;
		}
		if(isprint(s[i])){
			s[j]=s[i];
			j++;
		}
		i++;
	}
	s[j]='\0';
}

void i2c_lcd_print(char *s){
	byte i,j;
	byte lcd[9];
	
//	utf_del_uni(s);
	for(j=0;j<2;j++){
		lcd[8]='\0';
		for(i=0;i<8;i++){
			lcd[i]=(byte)s[i+8*j];
			if(lcd[i]==0x00){
				for(;i<8;i++) lcd[i]=' ';
				i2c_lcd_out(j,lcd);
				if(j==0){
					for(i=0;i<8;i++) lcd[i]=' ';
					i2c_lcd_out(1,lcd);
				}
				return;
			}
		}
		i2c_lcd_out(j,lcd);
	}
}
