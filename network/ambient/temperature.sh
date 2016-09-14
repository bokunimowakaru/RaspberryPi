#!/bin/bash
# Ambientへ Raspberry Piの温度データを送信する
# [保存先=Ambient(https://ambidata.io/)]
# Copyright (c) 2016 Wataru KUNINO

AmbientChannelId=100                                        # チャネルID(Ambientで取得)
AmbientWriteKey="0123456789abcdef"                          # ライトキー(16桁・同上)
HOST="54.65.206.59"                                         # 送信先アドレス(変更不要)
TEMP_OFFSET=25                                              # CPUの温度上昇値(要調整)
INTERVAL=30                                                 # 測定間隔(1日3000件以下)

while true;do                                               # 永久に繰り返し
temp=`cat /sys/devices/virtual/thermal/thermal_zone0/temp`  # 温度を取得
TEMP=$(( $temp / 100 - $TEMP_OFFSET * 10 ))                 # 温度に変換(10倍値)
DEC=$(( $TEMP / 10))                                        # 整数部
FRAC=$(( $TEMP - $DEC * 10))                                # 小数部
echo "Temperature = ${DEC}.${FRAC}"                         # 温度測定結果の表示
DATA=\"d1\"\:\"${DEC}.${FRAC}\"                             # データ生成
JSON="{\"writeKey\":\"${AmbientWriteKey}\",${DATA}}"        # JSON用のデータを生成
curl -s ${HOST}/api/v2/channels/${AmbientChannelId}/data\
     -X POST -H "Content-Type: application/json" -d ${JSON} # データ送信
sleep ${INTERVAL}                                           # 測定間隔の待ち時間
done                                                        # 繰り返し
