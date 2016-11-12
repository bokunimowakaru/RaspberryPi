#!/bin/bash
# Ambientから温度データを受信し、ファイルへ保存する。
# [データ取得元=Ambient(https://ambidata.io/)]
# Copyright (c) 2016 Wataru KUNINO

AmbientChannelId=100                                    # チャネルID(Ambient用)
AmbientReadKey="0123456789abcdef"                       # Readキー(16桁・同上)
HOST="ambidata.io"                                      # Ambientのアドレス
INTERVAL=60                                             # 測定間隔
DATE_="2000/01/01 00:00:00"                             # 前回の日付
DEV="ambie_1"                                           # デバイス名
if [ -f ~/.ambientkey ];then                            # 設定ファイル確認
    source ~/.ambientkeys
fi

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
if [ -n "$DATE" ];then                                  # データがあるとき
    DATE=`date --date "${DATE} UTC" "+%Y/%m/%d %R"`     # UTCをJST(等)へ変換
    if [ "$DATE" != "$DATE_" ];then                     # 前回の日時と異なるとき
        echo -n $DATE                                   # 時刻を表示
        echo -n ", Temperature =" $TEMP                 # 取得結果を表示
        echo -E ${DATE}, $TEMP >> log_${DEV}.csv        # ファイルへ保存する
        DATE_="$DATE"                                   # 日時のバックアップ
        echo " >>log_${DEV}.csv"                        # データ保存表示
    else
        echo                                            # 改行
    fi
fi
sleep ${INTERVAL}                                       # 測定間隔の待ち時間
done                                                    # 繰り返し
