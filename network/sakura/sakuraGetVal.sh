#!/bin/bash
# さくらのIoT Platform β からWebsocketで転送したデータを受信する
#
# http://blogs.yahoo.co.jp/bokunimowakaru/55588469.html
# 
# Copyright (c) 2016 Wataru KUNINO
#
################################################################################
#
# まず、さくらのIoT Platformβへアクセスし、「連携サービス」に「WebSocket」を
# 追加してください。
#
# https://secure.sakura.ad.jp/iot/
#
# 「Talken」が発行されるので、下記の「TALKEN=」の部分に記述してください。
#
TALKEN="xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"       # トークン

DEVICE="sakra_1"                                    # 識別名
INTERVAL = 3                                        # 確認間隔

while true; do
    DATA=`timeout $INTERVAL curl -s -N \
    -H 'Sec-WebSocket-Version: 13' \
    -H "Sec-WebSocket-Key: $(head -c 16 /dev/urandom | base64)" \
    -H "Connection: Upgrade" \
    -H "Upgrade: websocket" \
    https://api.sakura.io/ws/v1/${TALKEN}`
    # 参考文献 http://hateda.hatenadiary.jp/entry/debugging-websocket-using-curl
    if [ -n "$DATA" ]; then
        DATE=`echo $DATA\
        |tr ',' '\n'\
        |grep \"datetime\"\
        |head -1\
        |cut -c13-31\
        |tr "-" "/"\
        |tr "T" " "`
        DATE=`date --date "${DATE} UTC" "+%Y/%m/%d %H:%M:%S"` # UTCをJST(等)へ変換
        echo -n ${DATE}", "${DEVICE}", "
        VALUE=`echo $DATA\
        |tr '{' '\n'\
        |grep "\"channel\"\:0"\
        |head -1\
        |tr ',' '\n'\
        |grep \"value\"\
        |head -1\
        |cut -c9-`
        echo $VALUE
    fi
done
