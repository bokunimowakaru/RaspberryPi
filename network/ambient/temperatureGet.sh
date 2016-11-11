#!/bin/bash
# Ambientから温度データを受信し、UDPで送信する。ファイルへ保存する。
# [データ取得元=Ambient(https://ambidata.io/)]
# Copyright (c) 2016 Wataru KUNINO

AmbientChannelId=100                                    # チャネルID(Ambientで取得)
AmbientReadKey="0123456789abcdef"                       # Readキー(16桁・同上)
HOST="54.65.206.59"                                     # 送信先アドレス(変更不要)
TEMP_OFFSET=25                                          # CPUの温度上昇値(要調整)
INTERVAL=15                                             # 測定間隔
DATE_="2000/01/01 00:00:00"                             # 前回の日付
DEV="ambie_1"                                           # デバイス名を定義
IP=`hostname -I|tr " " "\n"|grep -Eo '([0-9]*\.){3}[0-9]*'\
|grep -v "127.0."|head -1|cut -d. -f1-3`                # IPアドレスを取得する

while true;do                                           # 永久に繰り返し
JSON=`curl -s \
"${HOST}/api/v2/channels/${AmbientChannelId}/data\?readKey=${AmbientReadKey}\&n=1"`
#echo $JSON
DATE=`echo $JSON\
    |tr "," "\n" \
    |grep "\"created\"" \
    |cut -c12-30 \
    |tr "-" "/"\
    |tr "T" " "`                                        # 日時データを抽出
TEMP=`echo $JSON|cut -d":" -f2|cut -d"," -f1`           # 温度データを抽出
echo -n $DATE
echo -n ", Temprature =" $TEMP                          # 取得結果を表示
if [ "$DATE" != "$DATE_" ];then                         # 前回の日時と異なるとき
    echo -E ${DEV},$TEMP > /dev/udp/${IP}.255/1024      # UDPで送信する
    echo -E ${DATE}, $TEMP >> log_${DEV}.csv            # ファイルへ保存する
    DATE_="$DATE"                                       # 日時のバックアップ
    echo " >>log_${DEV}.csv"                            # データ保存表示
else
    echo                                                # 改行
fi
sleep ${INTERVAL}                                       # 測定間隔の待ち時間
done                                                    # 繰り返し
