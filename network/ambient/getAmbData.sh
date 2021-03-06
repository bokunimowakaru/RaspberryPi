#!/bin/bash
# Ambientからデータを受信し、表示する。
# [データ取得元=Ambient(https://ambidata.io/)]
# Copyright (c) 2016 Wataru KUNINO

AmbientChannelId=100                                    # チャネルID(Ambient用)
AmbientReadKey="0123456789abcdef"                       # Readキー(16桁・同上)
HOST="ambidata.io"                                      # Ambientのアドレス

if [ "$#" -lt 1 ]; then
    echo "Usage: ${0} number, 0=all"
    NUM=3
else
    NUM=${1}
    NUM=`echo $NUM|awk '{printf "%d",$1}'`
fi
if [ $NUM -lt 0 ]; then
    NUM=$(( - NUM ))
fi

if [ $AmbientChannelId -le 100 ];then
    if [ -f ~/.ambientkeys ];then                           # 設定ファイル確認
        source ~/.ambientkeys                               # 設定ファイルロード済み
    else
        echo "AmbientからチャンネルIDとライトキー、リードキーを取得してください"
        echo "「.ambientkeys」へ以下のように設定しておくと便利です。"
        echo "AmbientChannelId=XXX"
        echo "AmbientReadKey=\"0123456789abcdef\""
        echo "AmbientWriteKey=\"0123456789abcdef\""
        exit 1
    fi
fi
echo "Ambient Channel Id =" $AmbientChannelId           # チャンネルID表示

if [ $NUM -eq 0 ] || [ $NUM -gt 100 ] ; then
    curl -s \
    "${HOST}/api/v2/channels/${AmbientChannelId}/data\?readKey=${AmbientReadKey}\&n=${NUM}"\
    > ambient_ch${AmbientChannelId}_tmp.json
    cat ambient_ch${AmbientChannelId}_tmp.json\
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
    rm -f ambient_ch${AmbientChannelId}_tmp.json
elif [ $NUM -gt 0 ]; then
    curl -s \
    "${HOST}/api/v2/channels/${AmbientChannelId}/data\?readKey=${AmbientReadKey}\&n=${NUM}"\
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
    done
fi
rm -f ambient_ch${AmbientChannelId}_tmp.json
