#!/bin/bash
# Ambientから温度データを受信し、ファイルへ保存する。
# [データ取得元=Ambient(https://ambidata.io/)]
# Copyright (c) 2016 Wataru KUNINO

AmbientChannelId=100                                    # チャネルID(Ambient用)
AmbientReadKey="0123456789abcdef"                       # Readキー(16桁・同上)
HOST="ambidata.io"                                      # Ambientのアドレス

if [ "$#" -lt 1 ]; then
    echo "Usage: ${0} number"
    exit
fi
if [ -f ~/.ambientkeys ];then                           # 設定ファイル確認
    source ~/.ambientkeys                               # 設定ファイルロード済み
fi
if [ $AmbientChannelId -le 100 ];then
    echo "AmbientからチャンネルIDとライトキー、リードキーを取得してください"
    echo "「.ambientkeys」へ以下のように設定しておくと便利です。"
    echo "AmbientChannelId=XXX"
    echo "AmbientReadKey=\"0123456789abcdef\""
    echo "AmbientWriteKey=\"0123456789abcdef\""
    exit -1
fi
echo "Ambient Channel Id =" $AmbientChannelId           # チャンネルID表示

curl -s \
"${HOST}/api/v2/channels/${AmbientChannelId}/data\?readKey=${AmbientReadKey}\&n=${1}"\
| tr -d "[{" \
| tr "}" "\n" \
| while read JSON; do
    DATE=`echo $JSON\
    |tr "," "\n" \
    |grep "\"created\"" \
    |cut -c12-30 \
    |tr "-" "/"\
    |tr "T" " "`                                        # 日時データを抽出
    if [ -n "$DATE" ];then                              # データがあるとき
        DATE=`date --date "${DATE} UTC" "+%Y/%m/%d %R"` # UTCをJST(等)へ変換
        echo -n ${DATE}" "                              # 時刻を表示
        for i in $(seq 1 8); do
            VAL=`echo $JSON\
            |tr "," "\n" \
            |grep "\"d${i}\"" \
            |cut -d":" -f2`                             # 値を抽出
            if [ "$VAL" ];then
                echo -n "d${i}="${VAL}" "
            fi
        done
        echo
    fi
done                                                    # 繰り返し
