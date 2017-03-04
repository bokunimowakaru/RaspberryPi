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
//  #define DEBUG

void delay(int i){
    while(i){
        usleep(1000);
        i--;
    }
}

int getCo2(char *port){
    uint8_t com[9]={0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
    uint8_t in[8], checksum=0x00;
    int i,co2;
    
    i=open_serial_port(9600,port);
    if(i<0){
        fprintf(stderr,"Usage : raspi_mhz19 (port; eg:/dev/ttyUSB0)\n");
        return -1;
    }
    delay(100);
    putb_serial_port(com,9);
    i=0;
    while(getch_serial_port() != 0xFF){
        delay(1);
        i++; 
        if(i>1000){
			fprintf(stderr,"Timed Out (%d)\n",i);
            return -1;
        }
    }
    for(i=0;i<8;i++){
        in[i]=getch_serial_port();
        checksum += in[i];
        delay(1);
    }
    close_serial_port();
    #ifdef DEBUG
    	for(i=0;i<8;i++) printf("0x%02x ",in[i]);
    	printf("(0x%02x)\n",checksum);
    #endif
    if(checksum){
		fprintf(stderr,"Check Sum Error (0x%02x)\n",checksum);
    	close_serial_port();
        return -1;
	}
    if(in[0]!=0x86){
		fprintf(stderr,"Command Error (0x%02x)\n",in[0]);
        return -1;
	}
	co2=(((int)in[1])<<8) + (int)in[2];
    return co2;
}

int main(int argc, char *argv[]){
	int co2;
	
    if(argc==2) co2=getCo2(argv[1]);
    else co2=getCo2("");
    if(co2<0){
        fprintf(stderr,"Usage : %s (port; eg:/dev/ttyUSB0)\n",argv[0]);
        printf("-1\n");
        return -1;
    }
    printf("%d\n",co2);
    return 0;
}

