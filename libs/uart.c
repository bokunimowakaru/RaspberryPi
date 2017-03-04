/**************************************************************************************
UART for Raspberry Pi

                                                  Copyright (c) 2015-2017 Wataru KUNINO
***************************************************************************************/

#include <stdio.h>                                  // 標準入出力用
#include <stdint.h>
#include <fcntl.h>                                  // シリアル通信用(Fd制御)
#include <termios.h>                                // シリアル通信用(端末IF)
#include <unistd.h>                                 // read,usleep用
#include <string.h>                                 // strncmp,bzero用
#include <sys/time.h>                               // fd_set,select用
#include <ctype.h>                                  // isprint,isdigit用
static int ComFd;                                   // シリアル用ファイルディスクリプタ
static struct termios ComTio_Bk;                    // 現シリアル端末設定保持用の構造体

#include "../libs/uart.h"

int open_serial_port(int speed_i,char *port){
    struct termios ComTio;                          // シリアル端末設定用の構造体変数
    speed_t speed = B115200;                        // 通信速度の設定
    char modem_dev[15]="/dev/ttyUSBx";              // シリアルポートの初期値
    int i;
    
    if( speed_i ==    600 ) speed = B600;
    if( speed_i ==   1200 ) speed = B1200;
    if( speed_i ==   2400 ) speed = B2400;
    if( speed_i ==   4800 ) speed = B4800;
    if( speed_i ==   9600 ) speed = B9600;
    if( speed_i ==  19200 ) speed = B19200;
    if( speed_i ==  38400 ) speed = B38400;
    if( speed_i ==  57600 ) speed = B57600;
    if( speed_i == 115200 ) speed = B115200;
    if( port[0] && strlen(port)<14 ){
        strcpy(modem_dev,port);
        ComFd=open(modem_dev, O_RDWR|O_NONBLOCK);   // シリアルポートのオープン
    }else for(i=12;i>=-1;i--){
        if(i>=10) snprintf(&modem_dev[5],8,"rfcomm%1d",i-10);   // ポート探索(rfcomm0-2)
        else if(i>=0) snprintf(&modem_dev[5],8,"ttyUSB%1d",i);  // ポート探索(USB0～9)
        else snprintf(&modem_dev[5],8,"ttyAMA0");   // 拡張IOのUART端子に設定
        ComFd=open(modem_dev, O_RDWR|O_NONBLOCK);   // シリアルポートのオープン
        if(ComFd >= 0) break;                       // forループを抜ける
    }
    if(ComFd >= 0){
    //  printf("com=%s\n",modem_dev);           // 成功したシリアルポートを表示
        tcgetattr(ComFd, &ComTio_Bk);           // 現在のシリアル端末設定状態を保存
        ComTio.c_iflag = 0;                     // シリアル入力設定の初期化
        ComTio.c_oflag = 0;                     // シリアル出力設定の初期化
        ComTio.c_cflag = CLOCAL|CREAD|CS8;      // シリアル制御設定の初期化
        ComTio.c_lflag = 0;                     // シリアルローカル設定の初期化
        bzero(ComTio.c_cc,sizeof(ComTio.c_cc)); // シリアル特殊文字設定の初期化
        cfsetispeed(&ComTio, speed);            // シリアル入力の通信速度の設定
        cfsetospeed(&ComTio, speed);            // シリアル出力の通信速度の設定
        ComTio.c_cc[VMIN] = 0;                  // リード待ち容量0バイト(待たない)
        ComTio.c_cc[VTIME] = 0;                 // リード待ち時間0.0秒(待たない)
        tcflush(ComFd,TCIFLUSH);                // バッファのクリア
        tcsetattr(ComFd, TCSANOW, &ComTio);     // シリアル端末に設定
    }else{
        fprintf(stderr,"Serial Open ERROR (%s)\n",modem_dev);
        return -1;
    //  exit(1);
    }
    return ComFd;
}

char getch_serial_port(void){
    char c='\0';                                    // シリアル受信した文字の代入用
    fd_set ComReadFds;                              // select命令用構造体ComReadFdを定義
    struct timeval tv;                              // タイムアウト値の保持用
    FD_ZERO(&ComReadFds);                           // ComReadFdの初期化
    FD_SET(ComFd, &ComReadFds);                     // ファイルディスクリプタを設定
    tv.tv_sec=0; tv.tv_usec=10000;                  // 受信のタイムアウト設定(10ms)
    if(select(ComFd+1, &ComReadFds, 0, 0, &tv)) read(ComFd, &c, 1); // データを受信
    usleep(5000);                                   // 5msの(IchigoJam処理)待ち時間
    return c;                                       // 戻り値＝受信データ(文字変数c)
}

int putch_serial_port(char c){
    return write(ComFd, &c, 1 );
}

int puts_serial_port(char *s){
    int i=0;
    while( s[i] != (char)0x00 ){
        write(ComFd, &s[i], 1 );
        i++;
    }
    return i;
}

int putb_serial_port(uint8_t *in,int len){
    return write(ComFd, in, len);
}

int close_serial_port(void){
    tcsetattr(ComFd, TCSANOW, &ComTio_Bk);
    return close(ComFd);
}
