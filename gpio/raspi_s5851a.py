#!/usr/bin/env python3
# coding: utf-8

################################################################################
# 温度センサ S-5851AAA-M6T1U（秋月電子通商製）から温度を取得します。
# https://bokunimo.net/ichigojam/i2c.html
#                                               Copyright (c) 2021 Wataru KUNINO
################################################################################

s5851 = 0x48

import smbus
from time import sleep

def word2uint(d1,d2):
    i = d1
    i <<= 8
    i += d2
    return i

i2c = smbus.SMBus(1)
while i2c:
    data=i2c.read_i2c_block_data(s5851,0x00,2)
    if len(data) >= 2:
        temp = float(word2uint(data[0],data[1])) / 256.
        print("Temp.= %.2f ℃" % (temp))
    sleep(1)
