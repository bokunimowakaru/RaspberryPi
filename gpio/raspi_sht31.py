#!/usr/bin/env python3
# coding: utf-8

################################################################################
# 温湿度センサ SENSIRION SHT31 から温度と湿度を取得します。
#
#                                               Copyright (c) 2021 Wataru KUNINO
################################################################################

sht31 = 0x45

import smbus
from time import sleep                               # 時間取得を組み込む

def word2int(d1,d2):
    i = d1
    i <<= 8
    i += d2
    if i >= 32768:
        i -= 65536
    return i

i2c = smbus.SMBus(1)
while i2c:
    i2c.write_byte_data(sht31,0x2C,0x06)
    sleep(0.018)
    data = i2c.read_i2c_block_data(sht31,0x00,6)
    if len(data) >= 5:
        temp = float(word2int(data[0],data[1])) / 65535. * 175. - 45.
        hum  = float(word2int(data[3],data[4])) / 65535. * 100.
        print("%.2f ℃, %.0f ％" % (temp,hum))
    sleep(1)
