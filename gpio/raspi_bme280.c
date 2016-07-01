/*******************************************************************************
Raspberry Pi用 I2C 温湿度気圧センサ Bosch BMP280 raspi_bmp280

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

I2C接続のセンサから測定値を取得する
Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                                        Copyright (c) 2014-2016 Wataru KUNINO
                                        http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

// usage: raspi_bmp280 [address]
//                      0x76    Lowの時
//                      0x77    HIghの時
//
// The last bit is changeable by SDO value and can be changed during operation.
// Connecting SDO to GND results in slave address 1110110 (0x76); 
// connection it to VDDIO results in slave address 1110111 (0x77), 
// which is the same as BMP280’s I²C address.

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "../libs/soft_i2c.h"
typedef unsigned char byte; 
typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;
typedef int64_t BME280_S64_t;
byte address=0x76;
#define debug

// BST-BME280-DS001-11 | Revision 1.2 | October 2015 Bosch Sensortec

/* singed integer type*/
typedef	int8_t s8;/**< used for signed 8bit */
typedef	int16_t s16;/**< used for signed 16bit */
typedef	int32_t s32;/**< used for signed 32bit */
typedef	int64_t s64;/**< used for signed 64bit */

typedef	u_int8_t u8;/**< used for unsigned 8bit */
typedef	u_int16_t u16;/**< used for unsigned 16bit */
typedef	u_int32_t u32;/**< used for unsigned 32bit */
typedef	u_int64_t u64;/**< used for unsigned 64bit */

u16 dig_T1;/**<calibration T1 data*/
s16 dig_T2;/**<calibration T2 data*/
s16 dig_T3;/**<calibration T3 data*/
u16 dig_P1;/**<calibration P1 data*/
s16 dig_P2;/**<calibration P2 data*/
s16 dig_P3;/**<calibration P3 data*/
s16 dig_P4;/**<calibration P4 data*/
s16 dig_P5;/**<calibration P5 data*/
s16 dig_P6;/**<calibration P6 data*/
s16 dig_P7;/**<calibration P7 data*/
s16 dig_P8;/**<calibration P8 data*/
s16 dig_P9;/**<calibration P9 data*/

u8  dig_H1;/**<calibration H1 data*/
s16 dig_H2;/**<calibration H2 data*/
u8  dig_H3;/**<calibration H3 data*/
s16 dig_H4;/**<calibration H4 data*/
s16 dig_H5;/**<calibration H5 data*/
s8  dig_H6;/**<calibration H6 data*/

s32 t_fine;/**<calibration T_FINE data*/
	
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T){
	BME280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) >> 12) *
	((BME280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P){
	BME280_S64_t var1, var2, p;
	var1 = ((BME280_S64_t)t_fine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)dig_P6;
	var2 = var2 + ((var1*(BME280_S64_t)dig_P5)<<17);
	var2 = var2 + (((BME280_S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (BME280_S64_t)dig_P3)>>8) + ((var1 * (BME280_S64_t)dig_P2)<<12);
	var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)dig_P1)>>33;
	if (var1 == 0) return 0; // avoid exception caused by division by zero
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BME280_S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BME280_S64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)dig_P7)<<4);
	return (BME280_U32_t)p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H){
	BME280_S32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) +
	((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r *
	((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
	((BME280_S32_t)dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (BME280_U32_t)(v_x1_u32r>>12);
}

int ahex2i(char c){
    if(c>='0' && c<='9') return c-'0';
    if(c>='a' && c<='f') return c-'a'+10;
    if(c>='A' && c<='F') return c-'F'+10;
    return -1;
}

uint16_t _getReg(byte data){
    i2c_write(address,&data,1);     	// 書込みの実行
    delay(10);
    i2c_read(address,&data,1);      	// 読み出し
    return (int)data;
}

void cal(){
	dig_T1 = (u16)(_getReg(0x88) + (_getReg(0x89)<<8));
	dig_T2 = (s16)(_getReg(0x8A) + (_getReg(0x8B)<<8));
	dig_T3 = (s16)(_getReg(0x8C) + (_getReg(0x8D)<<8));
	dig_P1 = (u16)(_getReg(0x8E) + (_getReg(0x8F)<<8));
	dig_P2 = (s16)(_getReg(0x90) + (_getReg(0x91)<<8));
	dig_P3 = (s16)(_getReg(0x92) + (_getReg(0x93)<<8));
	dig_P4 = (s16)(_getReg(0x94) + (_getReg(0x95)<<8));
	dig_P5 = (s16)(_getReg(0x96) + (_getReg(0x97)<<8));
	dig_P6 = (s16)(_getReg(0x98) + (_getReg(0x99)<<8));
	dig_P7 = (s16)(_getReg(0x9A) + (_getReg(0x9E)<<8));
	dig_P8 = (s16)(_getReg(0x9C) + (_getReg(0x9D)<<8));
	dig_P9 = (s16)(_getReg(0x9E) + (_getReg(0x9F)<<8));
	dig_H1 = (u8)(_getReg(0xA1));
	dig_H2 = (s16)(_getReg(0xE1) + (_getReg(0xE2)<<8));
	dig_H3 = (u8)(_getReg(0xE3));
	dig_H4 = (s16)((_getReg(0xE4)<<4) + (_getReg(0xE5)&0x0F));
	dig_H5 = (s16)(((_getReg(0xE5)&0xF0)>>4) + (_getReg(0xE6)<<4));
	dig_H6 = (s8)(_getReg(0xE7));
}

float getTemp(){
    int32_t in;
    in = _getReg(0xFA);            		// temp_msb[7:0]
    in <<= 8;
    in |= _getReg(0xFB);             	// temp_lsb[7:0]
    in <<= 4;
    in |= _getReg(0xFC); 				// temp_xlsb[3:0]
//    printf("getTemp  %08X %d\n",in,in);
	return ((float)BME280_compensate_T_int32(in))/100.;
}


float getHum(){
    int32_t in;
    in = _getReg(0xFD);            		// hum_msb[7:0]
    in <<= 8;
    in |= _getReg(0xFE);             	// hum_lsb[7:0]
//  printf("getHum   %08X\n",in);
	return ((float)bme280_compensate_H_int32(in))/1024.;
}

float getPress(){
    int32_t in;
    in = _getReg(0xF7);      			// press_msb[7:0]
    in <<= 8;
    in |= _getReg(0xF8);      			// press_lsb[7:0]
    in <<= 4;
    in |= _getReg(0xF9);				// press_xlsb[3:0]
//  printf("getPress %08X\n",in);
	return ((float)BME280_compensate_P_int64(in))/25600.;
}

int main(int argc,char **argv){
    byte config[2],in;
    int i;
    if( argc == 2 ){
        if( strlen(argv[1]) == 2 ){
            address=(byte)ahex2i(argv[1][0]);
            if(address>15) argc=0;
            else address<<=4;
            address+=(byte)ahex2i(argv[1][1]);
            if(address>=0x80) address>>=1;
        }else argc=0;
    }
    if( argc < 1 || argc > 2 ){
        fprintf(stderr,"usage: %s [i2c_address]\n",argv[0]);
        return -1;
    }

    i2c_init();
    cal();
    
    config[0]=0xF5;                 	// config
    config[1]=0b11000000;
    //          | || | |___________________ 触るな SCI切換え
    //          | ||_|_____________________ filter[2:0]
    //          |_|________________________ t_sb[2:0]
    i2c_write(address,config,2);    	// 書込みの実行
    
    config[0]=0xF2;                 	// trl_hum
    config[1]=0b00000001;
    //               |_|___________________ osrs_h[2:0]
    i2c_write(address,config,2);    	// 書込みの実行
    
    config[0]=0xF4;                 	// ctrl_meas
    config[1]=0b00100111;
    //          | || |||___________________ mode[1:0]
    //          | ||_|_____________________ osrs_p[2:0]
    //          |_|________________________ osrs_t[2:0]
    i2c_write(address,config,2);    	// 書込みの実行
    
    in=_getReg(0xD0);
	if(in != 0x58 && in != 0x60){
        fprintf(stderr,"Error chip_id (%02X)\n",in);
    	return -1;
	}
    for(i=0;i<10;i++){
		in=_getReg(0xF3);
	//	printf("getReg   %02X\n",in);
		if((in&0x04)==0) break;
		delay(100);
	}
    printf("%3.2f ",getTemp());
    printf("%3.2f ",getHum());
    printf("%4.2f\n",getPress());
    config[0]=0xF4;                 	// ctrl_meas
    config[1]=0x00;
    i2c_write(address,config,2);
    i2c_close();
    return 0;
}
