/*******************************************************************************
Raspberry Pi用 ソフトウェア I2C ライブラリ raspi_i2c / soft_i2c
Arduino ESP32 用 ソフトウェア I2C LCD ST7032i ドライバ soft_i2c

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

										Copyright (c) 2014-2023 Wataru KUNINO
										https://bokunimo.net/raspi/
							 			https://bokunimo.net/
********************************************************************************
元ファイル：
https://github.com/bokunimowakaru/RaspberryPi/blob/master/libs/soft_i2c.h
********************************************************************************
最新ファイル：
https://bokunimo.net/git/raspi_lcd/blob/master/raspi_i2c.h
*******************************************************************************/

//	通信の信頼性確保のため、戻り値の仕様を変更しました。
//	ヘッダファイルも変更しています。ご理解のほど、お願いいたします。
//	0:成功 1:失敗
//														2017/6/16	国野亘

#include <stdint.h>

typedef unsigned char byte;
#ifndef ARDUINO // ## for Raspberry Pi, Linux, Cygwin
    void delay(int i);
    byte pinMode(char *port, char *mode);
    byte digitalRead(char *port);
    byte digitalWrite(char *port, int value);
#endif
void i2c_debug(const char *s,byte priority);
void i2c_error(const char *s);
#ifndef ARDUINO // ## for Raspberry Pi, Linux, Cygwin
    byte i2c_hard_reset(int port);
    byte i2c_hard_quit(int port);
#endif
#ifdef ARDUINO
    void i2c_SCL(byte level);
    void i2c_SDA(byte level);
#else // ## for Raspberry Pi, Linux, Cygwin
    byte i2c_SCL(byte level);
    byte i2c_SDA(byte level);
#endif
byte i2c_tx(const byte in);
byte i2c_init(void);
byte i2c_close(void);
byte i2c_start(void);
byte i2c_check(byte adr);
byte i2c_read(byte adr, byte *rx, byte len);
byte i2c_write(byte adr, byte *tx, byte len);
byte i2c_lcd_out(byte y,byte *lcd);
void utf_del_uni(char *s);

// LCD 初期化
byte i2c_lcd_init(void);
byte i2c_lcd_init_xy(byte x, byte y);
void i2c_lcd_set_xy(byte x, byte y);
#ifdef ARDUINO
    void i2c_lcd_init_xy_sdascl(byte x,byte y,byte sda,byte scl);
#endif
byte i2c_lcd_set_fonts(const byte *s, int len);

// LCD 表示命令
byte i2c_lcd_print(const char *s);
byte i2c_lcd_print2(const char *s);
byte i2c_lcd_print_ip(uint32_t ip);
byte i2c_lcd_print_ip2(uint32_t ip);
byte i2c_lcd_print_val(const char *s,int in);
void time2txt(char *date,unsigned long local);
byte i2c_lcd_print_time(unsigned long local);
#ifdef ARDUINO // トランジスタ技術 2016.6 互換 API
    void lcdOut(byte y,byte *lcd);
    void lcdPrint(const char *s);
    void lcdPrint(String s);
    void lcdPrint2(const char *s);
    void lcdPrintIp(uint32_t ip);
    void lcdPrintIp2(uint32_t ip);
    void lcdPrintVal(const char *s,int in);
    void lcdPrintTime(unsigned long local);
    void lcdSetup(byte x, byte y, byte sda,byte scl);
    void lcdSetup(byte x, byte y);
    void lcdSetup();
#endif
