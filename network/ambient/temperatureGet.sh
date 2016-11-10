#!/bin/bash
# Ambientから温度データを受信する
# [保存先=Ambient(https://ambidata.io/)]
# Copyright (c) 2016 Wataru KUNINO

AmbientChannelId=100                                        # チャネルID(Ambientで取得)
AmbientReadKey="0123456789abcdef"                           # Readキー(16桁・同上)
HOST="54.65.206.59"                                         # 送信先アドレス(変更不要)
TEMP_OFFSET=25                                              # CPUの温度上昇値(要調整)
INTERVAL=15                                                 # 測定間隔

while true;do                                               # 永久に繰り返し
TEMP=`curl -s \
"${HOST}/api/v2/channels/${AmbientChannelId}/data\?readKey=${AmbientReadKey}\&n=1" \
|cut -d":" -f2|cut -d"," -f1`
echo "Temprature =" $TEMP
sleep ${INTERVAL}                                           # 測定間隔の待ち時間
done                                                        # 繰り返し
