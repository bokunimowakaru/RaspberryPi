#!/bin/bash
# HALTコマンドをUDPで受信してシャットダウンする (シャットダウン用サーバ)
# Copyright (c) 2018 Wataru KUNINO

# /etc/rc.local へ下記を追加する
# nohup /home/pi/RaspberryPi/network/udp_halt_serv.sh &>> /home/pi/start.log &

echo "UDP HALTER (usage: ${0} port)"                # タイトル表示
if [ ${#} = 1 ]                                     # 入力パラメータ数の確認
then                                                # 1つだった時
    if [ ${1} -ge 1 ] && [ ${1} -le 65535 ]         # ポート番号の範囲確認
    then                                            # 1以上65535以下の時
        PORT=${1}                                   # ポート番号を設定
    else                                            # 範囲外だった時
        PORT=1024                                   # UDPポート番号を1024に
    fi                                              # ifの終了
else                                                # 1つでは無かった時
    PORT=1024                                       # UDPポート番号を1024に
fi                                                  # ifの終了
echo "Listening UDP port "${PORT}"..."              # ポート番号表示
while true                                          # 永遠に
do                                                  # 繰り返し
    if [ ${PORT} -lt 1024 ]                         # ポート番号を確認
    then                                            # 1024未満の時
        UDP=`sudo netcat -luw0 ${PORT}`             # UDPパケットを取得
    else                                            # ポート番号が1024以上の時
        UDP=`netcat -luw0 ${PORT}`                  # UDPパケットを取得
    fi                                              # ifの終了
    DATE=`date "+%Y/%m/%d %R"`                      # 日時を取得
    DEV=${UDP#,*}                                   # デバイス名を取得(前方)
    DEV=${DEV%%,*}                                  # デバイス名を取得(後方)
    echo -E $DATE, $UDP|tee -a log_${DEV}.csv       # 取得日時とデータを表示
    echo -E $DATE, $UDP                             # 取得日時とデータを表示
    case "$DEV" in                                  # DEVの内容に応じて
        "uhalt_0" ) VAL=`echo -E $UDP|tr -d ' '|cut -d, -f2`
                    if [ $VAL -eq 1 ]; then         # 受信値が1のとき
                        sudo shutdown -h            # シャットダウンを実行
                    fi ;;
    esac
done                                                # 繰り返しここまで
