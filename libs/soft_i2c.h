/*******************************************************************************
Raspberry Pi用 ソフトウェアI2C ライブラリ  soft_i2c

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                               			Copyright (c) 2014-2017 Wataru KUNINO
                               			http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/

//	通信の信頼性確保のため、戻り値の仕様を変更しました。
//	ヘッダファイルも変更しています。ご理解のほど、お願いいたします。
//	0:成功 1:失敗
//														2017/6/16	国野亘

#include <stdint.h>

typedef unsigned char byte; 
void delay(int i);
byte pinMode(char *port, char *mode);
byte digitalRead(char *port);
byte digitalWrite(char *port, int value);
void i2c_debug(const char *s,byte priority);
void i2c_error(const char *s);
byte i2c_hard_reset(int port);
byte i2c_SCL(byte level);
byte i2c_SDA(byte level);
byte i2c_tx(const byte in);
byte i2c_init(void);
byte i2c_close(void);
byte i2c_start(void);
byte i2c_check(byte adr);
byte i2c_read(byte adr, byte *rx, byte len);
byte i2c_write(byte adr, byte *tx, byte len);
byte i2c_lcd_out(byte y,byte *lcd);
void utf_del_uni(char *s);
byte i2c_lcd_init(void);
byte i2c_lcd_init_xy(byte x, byte y);
byte i2c_lcd_print(char *s);
byte i2c_lcd_print2(char *s);
byte i2c_lcd_print_ip(uint32_t ip);
byte i2c_lcd_print_ip2(uint32_t ip);
byte i2c_lcd_print_val(char *s,int in);
byte i2c_lcd_print_time(unsigned long local);
