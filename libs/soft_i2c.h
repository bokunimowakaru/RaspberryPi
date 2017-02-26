/*******************************************************************************
Raspberry Pi用 ソフトウェアI2C ライブラリ  soft_i2c

本ソースリストおよびソフトウェアは、ライセンスフリーです。(詳細は別記)
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

Arduino標準ライブラリ「Wire」は使用していない(I2Cの手順の学習用サンプル)

                               			Copyright (c) 2014-2016 Wataru KUNINO
                               			http://www.geocities.jp/bokunimowakaru/
*******************************************************************************/
#include <stdint.h>

typedef unsigned char byte; 
void delay(int i);
void pinMode(char *port, char *mode);
byte digitalRead(char *port);
void digitalWrite(char *port, int value);
void i2c_debug(const char *s,byte priority);
void i2c_error(const char *s);
void i2c_SCL(byte level);
void i2c_SDA(byte level);
byte i2c_tx(const byte in);
byte i2c_init(void);
byte i2c_close(void);
byte i2c_start(void);
byte i2c_read(byte adr, byte *rx, byte len);
byte i2c_write(byte adr, byte *tx, byte len);
void i2c_lcd_out(byte y,byte *lcd);
void utf_del_uni(char *s);
void i2c_lcd_init(void);
void i2c_lcd_print(char *s);
void i2c_lcd_printIp(uint32_t ip);

