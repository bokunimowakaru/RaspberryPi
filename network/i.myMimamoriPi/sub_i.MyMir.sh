#!/bin/bash
#
# Raspberry Pi による見守りシステム
# IR リモコン信号受信モジュール部
# siMyMir_rxPi
# 
# Copyright (c) 2016 Wataru KUNINO

PID=`pidof raspi_ir_in`
kill ${PID} >& /dev/null
while true;do
IR=`../../gpio/raspi_ir_in 4`
echo -n "[IR] "
date +%d" "%H:%M |tr -d "\n" |tee -a ir.txt
echo ", "${IR} |tee -a ir.txt
done                                                        # 繰り返し
