#!/bin/bash
################################################################################
#
# さくらのIoT Platform β から IoTクラウドサービス Ambientへ転送する
#
# http://blogs.yahoo.co.jp/bokunimowakaru/55708303.html
# 
# Copyright (c) 2016-2017 Wataru KUNINO
#
################################################################################

# まず、さくらのIoT Platformβへアクセスし、「連携サービス」に「WebSocket」を
# 追加してください。
#
# https://secure.sakura.ad.jp/iot/
#
# 「Talken」が発行されるので、下記の「TALKEN=」の部分に記述してください。
#
TALKEN="xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"       # Sakura IoT トークン

#
# 次に、Ambientへアクセスし、ユーザ登録を行ってください。
# 既にユーザ登録済みの方は、チャネルIDを追加してください。
#
# https://ambidata.io/
#
# 「チャネルID」と「ライトキー」を、下記に記述してください。

AmbientChannelId=100                                # Ambient チャネルID
AmbientWriteKey="0123456789abcdef"                  # Ambient ライトキー(16桁)

DEVICE="sakra_1"                                    # 識別名
INTERVAL=20                                         # 送信間隔(20秒以上を推奨)

while true; do
    DATA=`curl --max-time $INTERVAL -s -N \
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
        VALUE=`echo $DATA\
        |tr '{' '\n'\
        |grep "\"channel\"\:0"\
        |head -1\
        |tr ',' '\n'\
        |grep \"value\"\
        |head -1\
        |cut -c9-`
        if [ -n "$VALUE" ]; then
            echo ${DATE}", "${DEVICE}", "${VALUE}
            JSON="{\"writeKey\":\"${AmbientWriteKey}\",\"d1\":\"${VALUE}\"}"
        #   echo $JSON
            curl -s ambidata.io/api/v2/channels/${AmbientChannelId}/data\
                 -X POST -H "Content-Type: application/json" -d ${JSON} # データ送信
        fi
    fi
done
