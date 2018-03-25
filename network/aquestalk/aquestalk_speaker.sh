#!/bin/bash
#
# Raspberry Pi版 ソフトウェア AquesTalk用 HTTP サーバ
# Copyright (c) 2017 Wataru KUNINO
#
# このスクリプトの実行には
# AquesTalkPi と nkf が必要です。
#
# AquesTalkPi のインストール方法
#       ./aquestalk_setup.sh
#
# nkf (Network Kanji Filter) のインストール方法
#       sudo apt-get install nkf
#
# 実行方法
#       ./aquestalk_speaker.sh &
#       または
#       nohup nice -18 /home/pi/esp/tools/aquestalk_speaker.sh 2>> /home/pi/err.log > /dev/null &
#                                               ※音が割れる場合はnicce -10～-20を追加する
#
# 再生方法
#       curl -s -m3 127.0.0.1/?TEXT="こんにちわ"
#       curl -s -m3 127.0.0.1/?TEXT="こんにちわ"&VOL=30
#                                               ※簡易実装につきTEXTよりVOLが後
#
# 実行を中断して再開した場合などに、エラーが表示され再生が出来ないことがあります。
# (古い while 処理が残っている場合)
# ブラウザのリロードボタンなどで、再実行してください。

IP=""                                                   # 本機のIPアドレス
TALK="日本語を話します。"                               # Web表示用
VOL=100                                                 # 音量(AquesTalk)
MIX=190                                                 # 音量(ミキサ)
MIX_VOL_ID=1                                            # 音量設定用のID

while [ ${#IP} -lt 11 ] || [ ${#IP} -gt 13 ]
do
    IP=`hostname -I|cut -d" " -f1`
    sleep 3
done
echo -E "IP="${IP}
amixer cset numid=${MIX_VOL_ID} ${MIX} &> /dev/null     # 音量(ミキサ)設定

HTML="\
HTTP/1.0 200 OK\n\
Content-Type: text/html\n\
Connection: close\n\
\n\
<html><head>\n\
<title>Test Page</title>\n\
<meta http-equiv=\"Content-type\" content=\"text/html; charset=UTF-8\">\n\
</head>\n\
<body>\n\
<h3>AquesTalkPi</h3>\n\
<form method=\"GET\" action=\"http://"${IP}"/\">\n\
<input type=\"text\" name=\"TEXT\" value=\"TALK\">\n\
<input type=\"submit\" value=\"送信\">\n\
</form>\n\
</html>\n\
\n\
"
aplay ../3_misc/sound/se_maoudamashii_chime10.wav &
sleep 0.5
aquestalkpi/AquesTalkPi '"起動しました。IPアドレスは'${IP}'です。"' |aplay &
(sleep 10; kill `pidof aplay`) &> /dev/null &
# echo -e $HTML|sed -e "s/\"TALK\"/\"${TALK}\"/g"       # HTML表示
echo "Listening HTTP port 80..."                        # ポート番号表示
while true                                              # 永遠に
do                                                      # 繰り返し
    echo -e $HTML\
    |sed -e "s/\"TALK\"/\"${TALK}\"/g"\
    |sudo netcat -lw1 -v 80\
    |while read TCP
    do
        DATE=`date "+%Y/%m/%d %R"`                      # 時刻を取得
        HTTP=`echo -E $TCP|cut -d"=" -f1`               # HTTPコマンドを抽出
        if [ "$HTTP" = "GET /?TEXT" ]; then
            echo -E $DATE, $TCP                         # 取得日時とデータを表示
            TALK=`echo -E $TCP\
            |cut -d"=" -f2\
            |cut -d" " -f1\
            |cut -d"&" -f1\
            |sed -e "s/+/ /g"\
            |nkf --url-input\
            |tr -d "\!\"\$\%\&\'\(\)\*\+\-\;\<\>\[\\\]\^\{\|\}"`    # 文字抽出
            # echo -E "TEXT="${TALK}
            HTTP2=`echo -E $TCP|cut -d"&" -f2|cut -d"=" -f1`
            if [ "$HTTP2" = "VOL" ]; then
                VOL=`echo -E $TCP\
                |cut -d"=" -f3\
                |cut -d" " -f1\
                |cut -d"&" -f1\
                |tr -dc [0-9]`
                if [ $VOL -gt 100 ]; then
                    VOL=100
                fi
                echo -E "VOL(Talk)="${VOL}
            fi
            kill `pidof aplay` &> /dev/null
            sleep 0.5
            aplay ../3_misc/sound/se_maoudamashii_chime10.wav &
            sleep 0.5
            aquestalkpi/AquesTalkPi -g ${VOL} "${TALK}"|aplay &     # 音声再生
            # (sleep 10; kill `pidof aplay`) &> /dev/null &
        elif [ "$HTTP" = "GET /?MIX" ]; then
            echo -E $DATE, $TCP                         # 取得日時とデータを表示
            MIX=`echo -E $TCP\
            |cut -d"=" -f2\
            |cut -d" " -f1\
            |cut -d"&" -f1\
            |tr -dc [0-9]`
            if [ $MIX -gt 255 ]; then
                VOL=255
            fi
            echo -E "VOL(Mixer)="${MIX}
            amixer cset numid=${MIX_VOL_ID} ${MIX} &> /dev/null     # 音量設定
        elif [ "$HTTP" = "GET /" ]; then
            echo -E $DATE, $TCP                         # 取得日時とデータを表示
        fi
    done
done                                                    # 繰り返しここまで
