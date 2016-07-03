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
if [[ -z ${IR} ]]; then
	echo "GPIOエラー：インストールまたは接続、実行方法が不適切です"
	echo
	echo "(1) setup.shをしましたか？"
	echo "　　cd ~/RaspberryPi/network/i.myMimamoriPi"
	echo "　　./setup.sh"
	echo "(2) GPIO 4 に赤外線受信モジュールを接続しましたか？"
	echo "(3) 実行方法"
	echo "　　cd ~/RaspberryPi/network/i.myMimamoriPi"
	echo "　　./i.MyMimamoriPi.sh"
	echo
	echo "Ctrl-Cでプログラムを止めてください"
	exit
fi
echo -n "[IR] "
date +%d" "%H:%M |tr -d "\n" |tee -a ir.txt
echo ", "${IR} |tee -a ir.txt
done                                                        # 繰り返し
