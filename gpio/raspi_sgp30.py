#!/usr/bin/env python3
# coding: utf-8

################################################################################
# CO2センサ SENSIRION SGP30 からCO2濃度を取得します。
#
#                                               Copyright (c) 2021 Wataru KUNINO
################################################################################

sgp30 = 0x58

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
i2c.write_byte_data(sgp30, 0x20, 0x03)
sleep(1.012)
while i2c:
    i2c.write_byte_data(sgp30, 0x20, 0x08)
    sleep(0.014)
    data=i2c.read_i2c_block_data(sgp30,0x00,6)
    if len(data) >= 5:
        co2 = word2int(data[0],data[1])
        tvoc= word2int(data[3],data[4])
        print("CO2= %d ppm, TVOC= %d ppb" % (co2,tvoc))
    sleep(1)
