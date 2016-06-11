#!/bin/bash
# Ambientへ Raspberry Piの温度データを送信する
# [保存先=Ambient(https://ambidata.io/)]
# Copyright (c) 2016 Wataru KUNINO

AmbientChannelId=100                                            # チャネルID
AmbientWriteKey="0123456789abcdef"                              # ライトキー(16桁)
HOST="54.65.206.59"                                             # 送信先アドレス(変更不要)
TEMP_OFFSET=30                                                  # CPUの温度上昇値(要調整)
INTERVAL=10                                                     # 測定間隔

while true;do                                                   # 永久に繰り返し
temp=`cat /sys/devices/virtual/thermal/thermal_zone0/temp`      # 温度を取得
DEC=`expr ${temp} / 1000`                                       # 気温に換算(整数部)
FRAC=`expr ${temp} / 100 - ${DEC} \* 10`                        # 気温に換算(小数部)
DEC=`expr ${DEC} - ${TEMP_OFFSET}`                              # 温度補正
echo "Temperature = ${DEC}.${FRAC}"                             # 温度測定結果の表示
DATA=\"d1\"\:\"`echo ${DEC}.${FRAC}`\"                          # データ生成
JSON="{\"writeKey\":\"${AmbientWriteKey}\",${DATA}}"            # JSON用のデータを生成
curl -s ${HOST}/api/v2/channels/${AmbientChannelId}/data\
     -X POST -H "Content-Type: application/json" -d ${JSON}     # データ送信
sleep ${INTERVAL}                                               # 測定間隔の待ち時間
done                                                            # 繰り返し
