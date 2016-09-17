#!/bin/bash
# Ambientへ Raspberry Piの温度データを送信する。ログを保存する。
# [保存先=Ambient(https://ambidata.io/), temperature.log]
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
DATA=\"d1\"\:\"$DEC.$FRAC\"                                 # データ生成
echo -n `date "+%Y/%m/%d %R"`", "|tee -a temperature.log    # 日付を出力
echo -n ${DEC}.${FRAC}", "|tee -a temperature.log           # 温度測定結果の表示
TEMP=`tail -10 temperature.log|tr -d ','|awk '{avr+=$3} END{printf"%.1f",avr/NR'}`
echo -n $TEMP|tee -a temperature.log                        # 平均値を表示
DATA=${DATA},\"d2\"\:\"$TEMP\"                              # データ生成
TIME=`ping -c1 google.com|tr -d '\n'|\
        awk -F'time=' '{print $2}'|cut -d' ' -f1`           # PING応答時間の測定
echo -n ", "${TIME}|tee -a temperature.log                  # PING応答時間の保存
DATA=${DATA},\"d3\"\:\"${TIME}\"                            # データ生成
IP=`hostname -I|tr '.' ' '|awk '{print $4}'`                # IPアドレスの取得
echo -n ", "${IP}|tee -a temperature.log                    # IPアドレスの保存
DATA=${DATA},\"d4\"\:\"${IP}\"                              # データ生成
JSON="{\"writeKey\":\"${AmbientWriteKey}\",${DATA}}"        # JSON用のデータを生成
#echo -n ", "$JSON
echo | tee -a temperature.log                               # 改行を保存
curl -s ${HOST}/api/v2/channels/${AmbientChannelId}/data\
     -X POST -H "Content-Type: application/json" -d ${JSON} # データ送信
sleep ${INTERVAL}                                           # 測定間隔の待ち時間
done                                                        # 繰り返し
