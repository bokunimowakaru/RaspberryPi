/**************************************************************************************
Raspberry Pi用 UART 二酸化炭素CO2 ガスセンサ Winsen MH-Z19 for Raspberry Pi

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

UART接続のWinsen MH-Z19センサから測定値を取得する

                                           Copyright (c) 2015-2017 Wataru KUNINO
                                           http://www.geocities.jp/bokunimowakaru/raspi/
***************************************************************************************/

#include <stdio.h>                                  // 標準入出力用
#include <unistd.h>                                 // usleep用
#include "../libs/uart.h"

void delay(int i){
    while(i){
        usleep(1000);
        i--;
    }
}

int main(int argc, char *argv[]){
    uint8_t com[9]={0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
    uint8_t in[8];
    int i;
    
    if(argc==2) i=open_serial_port(9600,argv[1]);
    else i=open_serial_port(9600,"");
    if(i<0){
        fprintf(stderr,"Usage : %s (port)",argv[0]);
        return -1;
    }
    delay(100);
    putb_serial_port(com,9);
    i=0;
    while(getch_serial_port() != 0xFF){
        delay(10); i++; 
        /*
        if(i>100){
            printf("-1\n");
            return -1;
        }
        */
    }
    for(i=0;i<8;i++){
        in[i]=getch_serial_port();
        delay(1);
    }
    for(i=0;i<8;i++) printf("0x%02x ",in[i]);
    printf("\n");
    close_serial_port();
    return 0;
}

