/*******************************************************************************
Raspberry Pi用 I2C 揮発性有機化合物センサ AMS CCS811 raspi_ccs811.c

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する
AMS CCS811 Carbon Monoxide CO VOCs Air Quality Numerical Gas Sensors

揮発性有機化合物 VOCガス
二酸化炭素CO2(相当)
                                        Copyright (c) 2014-2017 Wataru KUNINO
                                        https://bokunimo.net/raspi/
*******************************************************************************/

// usage: raspi_ccs811 [i2c_address]
//                      0x5A    When ADDR is low
//                      0x5B    When ADDR is high

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
byte i2c_address=0x5A;

// #define DEBUG

uint8_t _ccs811_getByte(byte reg){
    uint8_t tx=reg;
    uint8_t rx;
    i2c_write(i2c_address,&tx,1);       // 書込みの実行
    delay(10);
    i2c_read(i2c_address,&rx,1);
    #ifdef DEBUG
    //  printf("rx=%02x\n",rx);
    #endif
    return rx;
}

int _ccs811_getReg(byte reg, byte *rx, int len){
    uint8_t tx=reg;
    int i;
    if(len < 0 || len>8) return -1;
    i2c_write(i2c_address,&tx,1);       // 書込みの実行
    delay(10);
    i=i2c_read(i2c_address,(byte *)rx,len);
    #ifdef DEBUG
        printf("rx[%d]=",i);
        for(i=0;i<len;i++){
            printf("%02x ",rx[i]);
        }
        printf("\n");
    #endif
    return i;
}

int _ccs811_setAppStart(){
    uint8_t tx=0xF4;                    // 0xF4 APP_START W - Application start.
    return i2c_write(i2c_address,&tx,1);       // 書込みの実行
}

int _ccs811_setMode(byte mode){
    uint8_t tx[2]={0x01,0x00};          // MEAS_MODE (Measurement and Conditions) Register (0x01)
    if(mode>3) return -1;
    tx[1] |= mode<<4;
    return i2c_write(i2c_address,tx,2);        // 書込みの実行
}

int _ccs811_getVals(){
    uint8_t tx=0x02;                    // 0x02 ALG_RESULT_DATA
    uint8_t rx[8];
    int i,len,adc;
    #ifdef DEBUG
        int current;
    #endif
    
    i2c_write(i2c_address,&tx,1);       // 書込みの実行
    delay(20);
    len=i2c_read(i2c_address,rx,8);
    
    i=((int)rx[6])*256+(int)rx[7];
    adc = i & 0x03FF;
    #ifdef DEBUG
        current = i>>10;
        if(len==8){
            printf("------------------------\n");
            printf("RESULT_DATA\n");
            printf("------------------------\n");
            printf("eCO2        =0x%02X%02X (%d)\n",rx[0],rx[1],((int)rx[0])*256+(int)rx[1]);
            printf("TVOC        =0x%02X%02X (%d)\n",rx[2],rx[3],((int)rx[2])*256+(int)rx[3]);
            printf("STATUS      =0x%02X\n",rx[4]);
            printf("ERROR_ID    =0x%02X\n",rx[5]);
            printf("RAW_DATA    =0x%02X%02X (%d)\n",rx[6],rx[7],i);
            printf("Current     =0x%02X (%d)\n",current,current);
            printf("Raw ADC     =0x%04X (%d)\n",adc,adc);
        }else{
            printf("rx[%d]=",len);
            for(i=0;i<len;i++){
                printf("%02x ",rx[i]);
            }
            printf("\n");
        }
    #endif
    if(len!=8)return -1;
    return adc;
}

int setEnv(float temp, float hum){
    uint8_t tx[5]={0x05,0,0,0,0};       // ENV_DATA (Environment Data) Register (0x05)
    uint16_t val;
    val = (uint16_t)(hum * 512.);
    tx[1]= (uint8_t)(val >> 8);
    tx[2]= (uint8_t)(val && 0xFF);
    val = (uint16_t)((temp + 25.) * 512.);
    tx[3]= (uint8_t)(val >> 8);
    tx[4]= (uint8_t)(val && 0xFF);
    return i2c_write(i2c_address,tx,5); // 書込みの実行
}

int getCO2(){                           // 二酸化炭素濃度（ppm)を取得
    uint8_t tx=0x02;                    // 0x02 ALG_RESULT_DATA
    uint8_t rx[2];
    i2c_write(i2c_address,&tx,1);
    delay(20);
    if(i2c_read(i2c_address,rx,2) != 2) return -1;
    return ((int)rx[0])*256+(int)rx[1];
}

int setup(){
    byte ret,id,status,mode;
    
    i2c_init();
    id=_ccs811_getByte(0x20);           // HW_ID
    #ifdef DEBUG
        printf("HW_ID       =0x%02X\n",id);
    #endif
    if(id != 0x81){
        fprintf(stderr,"ERROR: unknown device\n");
        printf("-1\n");
        i2c_close();
        exit(-1);
    }
    
    setEnv(25.0,50.0);                  // 補正用の温度と湿度を設定
    
    ret=_ccs811_setAppStart();
    #ifdef DEBUG
        printf("AppStart    =%1X\n",ret);
    #endif
    if(ret==0){
        fprintf(stderr,"ERROR: failed to start app\n");
        printf("-1\n");
        i2c_close();
        exit(-1);
    }
    
    status=_ccs811_getByte(0x00);       // STATUS
    #ifdef DEBUG
        printf("------------------------\n");
        printf("STATUS      =0x%02X\n",status);
        printf("------------------------\n");
        printf("FW_MODE     =%1X\n",(status & 0x80) >> 7);
        printf("APP_VALID   =%1X\n",(status & 0x10) >> 4);
        printf("DATA_READY  =%1X\n",(status & 0x08) >> 3);
        printf("ERROR       =%1X",(status & 0x01));
        if(status & 0x01){
            printf(" (0x%02X)\n",_ccs811_getByte(0xE0));
        }else printf("\n");
    #endif
    if( (status & 0x80) == 0){
        fprintf(stderr,"ERROR: failed APP starting\n");
        printf("-1\n");
        i2c_close();
        exit(-1);
    }
    
    ret=_ccs811_setMode(1);             // 測定開始 1:1秒ごとに測定 2:10秒、3:1分
    #ifdef DEBUG
        printf("MeasureStart=%1X\n",(ret>0));
    #endif
    
    delay(1000);
    mode=_ccs811_getByte(0x01);         // MEAS_MODE
    #ifdef DEBUG
        printf("------------------------\n");
        printf("MEAS_MODE   =0x%02X\n",mode);
        printf("------------------------\n");
        printf("DRIVE_MODE  =%1X\n",(mode & 0x70) >> 4);
        printf("INT_DATARDY =%1X\n",(mode & 0x08) >> 3);
        printf("INT_THRESH  =%1X\n",(mode & 0x04) >> 2);
    #endif
    if((mode & 0x70) == 0x00){
        fprintf(stderr,"ERROR: failed to set mode (%d)\n",(mode & 0x70)>>4);
        printf("-1\n");
        i2c_close();
        exit(-1);
    }
    return 0;
}

int main(int argc,char **argv){
    int co2=0;
    
    if( argc == 2 ) i2c_address=(byte)strtol(argv[1],NULL,16);
    if( i2c_address>=0x80 ) i2c_address>>=1;
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s [i2c_address]\n",argv[0]);
        return -1;
    }
    #ifdef DEBUG
        printf("i2c_address =0x%02X\n",i2c_address);
    #endif

    setup();
    while(co2==0){
        co2=getCO2();
        if(co2>0) break;
        #ifdef DEBUG
        if(co2==0) co2=_ccs811_getVals();
        #endif
        delay(1000);
    }
    printf("%d\n",co2);
    i2c_close();
    return 0;
}
