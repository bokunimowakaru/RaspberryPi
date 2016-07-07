#!/bin/bash
#
# Raspberry Pi による見守りシステム i.myMimamoriPi
#
# Copyright (c) 2016 Wataru KUNINO

MAILTO="xbee@dream.jp"                                          # メール送信先
TEMP_OFFSET=0 #(℃)                                             # 内部温度上昇補正
MONITOR_START=7 #(時)                                           # 監視開始時刻
MONITOR_END=21 #(時)                                            # 監視終了時刻
REPORT_TIME=9 #(時)                                             # メール送信時刻
REPORT_STAT=0                                                   # メール送信状態
ALLOWED_TERM=4 #(時間)                                          # 警報指定時間(22以下)
ALLOWED_TEMP=35 #(℃)                                           # 警報指定温度

# 初期化
echo "i.myMimamoriPi"                                           # タイトル表示
PID=`ps -eo pid,cmd|awk '/.*sub_i.MyMir.sh$/{print$1}'`         # 子プロセスのID確認
kill ${PID} >& /dev/null                                        # 子プロセス停止
sleep 0.1                                                       # 停止待ち
PID=`pidof raspi_ir_in`                                         # 子プロセスのID確認
kill ${PID} >& /dev/null                                        # 赤外線受信停止
./sub_i.MyMir.sh &                                              # 赤外線受信の開始
# 監視処理
while true;do                                                   # ループ処理の開始
    # データ取得処理
    time=`date +%d" "%H:%M`                                     # 時刻を変数timeへ
    hour=`echo $time|cut -c4-5`                                 # 「時」をhourへ
    if [ ${hour:0:1} = 0 ]; then hour=${hour:1:1}; fi           # 先頭0を削除
    min=`echo $time|cut -c7-8`                                  # 「分」をminへ
    # 使用する温度センサに合わせて選択する(Apple PiはBOSCH BME 280)
    temp=`../../gpio/raspi_bme280|cut -d"." -f1`                # BOSCH BME280使用時
#   temp=`../../gpio/raspi_temp|cut -d"." -f1`                  # CPU内蔵センサ用
#   temp=`../../gpio/raspi_am2320|cut -d"." -f1`                # Aosong AM2320
#   temp=`../../gpio/raspi_lps25h|cut -d"." -f1`                # STマイクロ LPS25H
#   temp=`../../gpio/raspi_stts751|cut -d"." -f1`               # STマイクロ STTS751
    ((temp -= TEMP_OFFSET))                                     # 温度の補正
    IR=`tail -1 ir.txt|cut -c4-5`                               # 赤外線の操作を取得
    if [ ${IR:0:1} = 0 ]; then IR=${IR:1:1}; fi                 # 先頭0を削除
    echo -n "[Mi] Time="${time}", "                             # 動作表示(時刻)
    echo "Temperature="${temp}", IR="${IR}                      # 動作表示(温度)
    ../../gpio/raspi_lcd -i ${time}${temp}ﾟC ${IR}              # 液晶へ表示
    # 10分ごとの処理
    if [ ${min:1:1} = 0 ]; then                                 # 分の下桁が0のとき
        # 判定処理
        mes=0                                                   # メッセージ初期化
        if [ ${hour} = 0 ]; then REPORT_STAT=0; fi              # メール状態の解除
        if [ ${hour} = $REPORT_TIME -a $REPORT_STAT = 0 ]; then # メール送信時刻検出
            mes="現在の状態のお知らせ"                          # 送信メッセージ
            REPORT_STAT=1                                       # 送信済に設定
        fi
        ((trig = hour - IR))                                    # 経過時間を計算
        if [ ${trig} -lt 0 ]; then ((trig += 24)); fi           # マイナス時の処理
        if [ ${hour} -ge $MONITOR_START -a ${hour} -le $MONITOR_END ]; then
            if [ ${trig} -ge $ALLOWED_TERM ];then mes="長時間、操作がありません";fi
            if [ ${temp} -ge $ALLOWED_TEMP ];then mes="室温が高くなっています";fi
        fi                                                      # 警告状態の判定処理
        # 通知処理
        if [ ${mes} != 0 ]; then                                # 警告がある時
            echo "[Mi] Message="${mes}                          # 動作表示(警告)
            text="Date,Time="${time}"\nTemperature="${temp}"℃\nIR="`tail -1 ir.txt`
            if [ ${trig} -ge 3 ]; then                          # 3時間以上のとき
            text=${text}"\n最後にリモコンを操作してから"${trig}"時間が経過しました"
            fi
            echo -e ${text}                                     # 動作表示(メール)
            echo -e ${text} | mutt -s ${mes} $MAILTO            # メール送信の実行
        fi
    fi
    # 待機処理(分表示が変わるまで待機する)
    min2=`date +%M`                                             # 現在の分を取得
    while [ ${min} = ${min2} ]; do                              # 変化が無ければ、
        sleep 1                                                 # 1秒間スリープし、
        min2=`date +%M`                                         # 分を再取得
    done                                                        # 繰り返し(スリープ)
done                                                            # 繰り返し(監視処理)
