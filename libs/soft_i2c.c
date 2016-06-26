/*******************************************************************************
Raspberry Pi用 ソフトウェアI2C ライブラリ  soft_i2c

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                               			Copyright (c) 2014-2016 Wataru KUNINO
                               			http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>						// uint32_t
#include <unistd.h>         			// usleep用
#include <ctype.h>						// isprint用
#include <sys/time.h>					// gettimeofday用
#include <string.h>						// strncpy用

#define PORT_SCL	"/sys/class/gpio/gpio3/value"		// I2C SCLポート
#define PORT_SDA	"/sys/class/gpio/gpio2/value"		// I2C SDAポート
#define PORT_SDANUM	2									// I2C SDAポートの番号
														// SCLはSDA+1(固定)
#define INPUT		"in"
#define OUTPUT		"out"
#define LOW			0
#define HIGH		1
#define	I2C_RAMDA	30					// I2C データシンボル長[us]
#define GPIO_RETRY  50      			// GPIO 切換え時のリトライ回数
#define S_NUM       16       			// 文字列の最大長
//	#define DEBUG               		// デバッグモード

typedef unsigned char byte; 
FILE *fgpio;
char buf[S_NUM];
struct timeval micros_time;				//time_t micros_time;
int micros_prev,micros_sec;

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

void i2c_debug(const char *s,byte priority){
	if(priority>3)	fprintf(stderr,"[%10d] ERROR:%s\n",_micros(),s);
    #ifdef DEBUG
	else 			fprintf(stderr,"[%10d]      :%s\n",_micros(),s);
    #endif
}

void i2c_error(const char *s){
	i2c_debug(s,5);
}
void i2c_log(const char *s){
	i2c_debug(s,1);
}

void pinMode(char *port, char *mode){
	int i=0;
	char dir[]="/sys/class/gpio/gpio3/direction";
	         // 0123456789012345678901234567890
	dir[20]=port[20];
    #ifdef DEBUG
    //	fprintf(stderr,"pinMode %s %s\n",dir,mode);
    #endif
	while(i<GPIO_RETRY){
		fgpio = fopen(dir, "w");
		if(fgpio){
			fprintf(fgpio,mode);
			fclose(fgpio);
			break;
		}
		delay(1);
		i++;
	}
    #ifdef DEBUG
    //	fprintf(stderr,"pinMode / GPIO_RETRY (%d/%d)\n",i,GPIO_RETRY);
    #endif
}

byte digitalRead(char *port){
    fgpio = fopen(port, "r");
	if( fgpio ){
	    fgets(buf, S_NUM, fgpio);
	    fclose(fgpio);
	}
    #ifdef DEBUG
    //	fprintf(stderr,"digitalRead %s %s\n",port,buf);
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
    //	fprintf(stderr,"digitalWrite %s %d\n",port,value);
    #endif
}

void i2c_SCL(byte level){
	if( level ){
		pinMode(PORT_SCL, INPUT);
	}else{
		pinMode(PORT_SCL, OUTPUT);
		digitalWrite(PORT_SCL, LOW);
	}
	_delayMicroseconds(I2C_RAMDA);
}

void i2c_SDA(byte level){
	if( level ){
		pinMode(PORT_SDA, INPUT);
	}else{
		pinMode(PORT_SDA, OUTPUT);
		digitalWrite(PORT_SDA, LOW);
	}
	_delayMicroseconds(I2C_RAMDA);
}

byte i2c_tx(const byte in){
	int i;
    #ifdef DEBUG
    	char s[32];
		sprintf(s,"tx data = [%02X]",in);
		i2c_log(s);
    #endif
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
	for(i=GPIO_RETRY;i>0;i--){
		if( digitalRead(PORT_SDA) == 0 ) break;	// 速やかに確認
		_delayMicroseconds(I2C_RAMDA);
	}
	if(i==0){
		i2c_error("I2C_TX / no ACK");
		return(0);
	}
    #ifdef DEBUG
    //	fprintf(stderr,"i2c_tx / GPIO_RETRY (%d/%d)\n",GPIO_RETRY-i,GPIO_RETRY);
    #endif
	return(i);
}

byte i2c_init(void){
	int i;

	_micros_0();
	i2c_log("I2C_Init");
    for(i=0;i<2;i++){
		fgpio = fopen("/sys/class/gpio/export","w");
	    if(fgpio==NULL ){
	        i2c_error("I2C_Init / IO Settiong Error\n");
	        printf("9\n");
	        return i;
	    }
	    fprintf(fgpio,"%d\n",i + PORT_SDANUM);
	    fclose(fgpio);
	}
	for(i=GPIO_RETRY;i>0;i--){						// リトライ50回まで
		i2c_SDA(1);							// (SDA)	H Imp
		i2c_SCL(1);							// (SCL)	H Imp
		if( digitalRead(PORT_SCL)==1 &&
			digitalRead(PORT_SDA)==1  ) break;
		delay(1);
	}
	if(i==0) i2c_error("I2C_Init / Locked Lines");
    #ifdef DEBUG
    //	fprintf(stderr,"i2c_init / GPIO_RETRY (%d/%d)\n",GPIO_RETRY-i,GPIO_RETRY);
    #endif
	_delayMicroseconds(I2C_RAMDA*8);
	return(i);
}

byte i2c_close(void){
	byte i;
	i2c_log("i2c_close");
    for(i=0;i<2;i++){
		fgpio = fopen("/sys/class/gpio/unexport","w");
	    if(fgpio==NULL ){
	        fprintf(stderr,"IO Error\n");
	        printf("9\n");
	        return -1;
	    }
	    fprintf(fgpio,"%d\n",i + PORT_SDANUM);
	    fclose(fgpio);
	}
	return 0;
}

byte i2c_start(void){
//	if(!i2c_init())return(0);				// SDA,SCL  H Out
	int i;

	for(i=5000;i>0;i--){						// リトライ50回まで
		i2c_SDA(1);							// (SDA)	H Imp
		i2c_SCL(1);							// (SCL)	H Imp
		if( digitalRead(PORT_SCL)==1 &&
			digitalRead(PORT_SDA)==1  ) break;
		delay(1);
	}
	i2c_log("i2c_start");
	if(i==0) i2c_error("i2c_start / Locked Lines");
	_delayMicroseconds(I2C_RAMDA*8);
	i2c_SDA(0);								// (SDA)	L Out
	_delayMicroseconds(I2C_RAMDA);
	i2c_SCL(0);								// (SCL)	L Out
	return(1);
}

byte i2c_read(byte adr, byte *rx, byte len){
/*
入力：byte adr = I2Cアドレス(7ビット)
出力：byte *rx = 受信データ用ポインタ
入力：byte len = 受信長
戻り値：byte 受信結果長、０の時はエラー
*/
	byte ret,i;
	
	if( !i2c_start() ) return(0);
	adr <<= 1;								// 7ビット->8ビット
	adr |= 0x01;							// RW=1 受信モード
	if( i2c_tx(adr)==0 ) return(0);			// アドレス設定
	
	/* スレーブ待機状態待ち */
	for(i=GPIO_RETRY;i>0;i--){
		_delayMicroseconds(I2C_RAMDA);
		if( digitalRead(PORT_SDA)==0  ) break;
	}
	if(i==0){
		i2c_error("I2C_RX / no ACK");
		return(0);
	}
	for(i=10;i>0;i--){
		_delayMicroseconds(I2C_RAMDA);
		if( digitalRead(PORT_SCL)==1  ) break;
	}
	if(i==0){
		i2c_error("I2C_RX / Clock Line Holded");
		return(0);
	}
	/* 受信データ */
	for(ret=0;ret<len;ret++){
		i2c_SCL(0);							// (SCL)	L Out
		i2c_SDA(1);							// (SDA)	H Imp
		rx[ret]=0x00;
		for(i=0;i<8;i++){
			i2c_SCL(1);						// (SCL)	H Imp
			rx[ret] |= (digitalRead(PORT_SDA))<<(7-i);		//data[22] b4=Port 12(SDA)
			i2c_SCL(0);						// (SCL)	L Out
		}
		// ACKを応答する
		i2c_SDA(0);							// (SDA)	L Out
		i2c_SCL(1);							// (SCL)	H Imp
		_delayMicroseconds(I2C_RAMDA);
	}
	i2c_SDA(1);								// (SDA)	H Imp
	return(ret);
}

byte i2c_write(byte adr, byte *tx, byte len){
/*
入力：byte adr = I2Cアドレス(7ビット)
入力：byte *tx = 送信データ用ポインタ
入力：byte len = 送信データ長（0のときはアドレスのみを送信する）
*/
	byte ret=0;
	if( !i2c_start() ) return(0);
	adr <<= 1;								// 7ビット->8ビット
	adr &= 0xFE;							// RW=0 送信モード
	if( i2c_tx(adr)>0 && len>0 ){
		/* データ送信 */
		for(ret=0;ret<len;ret++){
			i2c_SDA(0);						// (SDA)	L Out
			i2c_SCL(0);						// (SCL)	L Out
			i2c_tx(tx[ret]);
		}
	}
	/* STOP */
	i2c_SDA(0);								// (SDA)	L Out
	i2c_SCL(0);								// (SCL)	L Out
	_delayMicroseconds(I2C_RAMDA);
	if(len==0)_delayMicroseconds(800);
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
	i2c_write(0x3E,data,2);
	for(i=0;i<8;i++){
		if(lcd[i]==0x00) break;
		data[0]=0x40;
		data[1]=lcd[i];
		i2c_write(0x3E,data,2);
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
		// fprintf(stderr,"%02X ",s[i]);
		if(isprint(s[i]) || (s[i]>=0xA1 && s[i] <=0xDF)){
			s[j]=s[i];
			j++;
		}
		i++;
	}
	s[j]='\0';
	// fprintf(stderr,"len=%d\n",j);
}

void i2c_lcd_init(void){
	byte data[2];
	data[0]=0x00; data[1]=0x39; i2c_write(0x3E,data,2);	// IS=1
	data[0]=0x00; data[1]=0x11; i2c_write(0x3E,data,2);	// OSC
	data[0]=0x00; data[1]=0x70; i2c_write(0x3E,data,2);	// コントラスト	0
	data[0]=0x00; data[1]=0x56; i2c_write(0x3E,data,2);	// Power/Cont	6
	data[0]=0x00; data[1]=0x6C; i2c_write(0x3E,data,2);	// FollowerCtrl	C
	delay(200);
	data[0]=0x00; data[1]=0x38; i2c_write(0x3E,data,2);	// IS=0
	data[0]=0x00; data[1]=0x0C; i2c_write(0x3E,data,2);	// DisplayON	C
}

void i2c_lcd_print(char *s){
	byte i,j;
	char str[49];
	byte lcd[9];
	
	strncpy(str,s,48);
	utf_del_uni(str);
	for(j=0;j<2;j++){
		lcd[8]='\0';
		for(i=0;i<8;i++){
			lcd[i]=(byte)str[i+8*j];
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

void i2c_lcd_printIp(uint32_t ip){
	char lcd[17];
	
	sprintf(lcd,"%i.%i.    ",
		ip & 255,
		ip>>8 & 255
	);
	sprintf(&lcd[8],"%i.%i",
		ip>>16 & 255,
		ip>>24
	);
	i2c_lcd_print(lcd);
}
