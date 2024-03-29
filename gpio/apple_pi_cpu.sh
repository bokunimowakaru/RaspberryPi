#!/bin/bash
################################################################################
# raspi_lcd Version 1.01以降用 サンプル・ソフトウェア
# LCD AQM0802 (8桁×2行)
#                                       Copyright (c) 2022 - 2023 Wataru KUNINO
################################################################################

# (参考) 自動起動：
#   /etc/rc.localに追加する場合
#       su -l pi -s /bin/bash -c /home/pi/RaspberryPi/gpio/apple_pi_cpu.sh &> /dev/null &
#       実行権限の付与が必要：
#       $ chmod u+x /etc/rc.local ⏎
#   crontabに追加する場合
#       @reboot /home/pi/RaspberryPi/gpio/apple_pi_cpu.sh &> /dev/null &
#

BUTTON_IO="27"  # ボタンでシャットダウン(使用しないときは0)
LCD_IO="16"                 # LCD用電源用IOポート番号を指定する
LCD_APP="./raspi_lcd"       # LCD表示用。※要makeの実行
LOG="/dev/stdout"           # ログファイル名(/dev/stdoutでコンソール表示)
if [ ! -x $LCD_APP ]; then
    LCD_APP=`which raspi_lcd`
    if [ -z "${LCD_APP}" ] || [ ! -x $LCD_APP ]; then
        LCD_APP="/home/pi/raspi_lcd/raspi_lcd"
        if [ ! -x $LCD_APP ]; then
            LCD_APP="/home/pi/RaspberryPi/gpio/raspi_lcd"
fi fi fi

SECONDS=0
MEMS=(100 100 100 100 100 100 100 100)
CPU_CORES=`cat /proc/cpuinfo|grep "processor"|wc -l`

date4log (){
    date +"%Y/%m/%d %H:%M:%S"
}

# ボタン状態を取得 (取得できないときは0,BUTTON_IO未設定時は1)
button (){
    if [ $(($BUTTON_IO)) -le 0 ]; then
        return 1
    else
        return $((`raspi-gpio get ${BUTTON_IO}|awk '{print $3}'|sed 's/level=//g'`))
    fi
}
button_shutdown (){
    if [ $(($BUTTON_IO)) -gt 0 ]; then
        button
        if [ $? -eq 0 ]; then
            $LCD_APP -i `hostname -I|cut -d" " -f1` >/dev/null
            sleep 0.5
            button
            if [ $? -eq 0 ]; then
                echo `date4log` "[WARNING] Pressed Shutdown Button" >> $LOG 2>&1
                $LCD_APP -i "Pressed PowerBtn" > /dev/null
                sleep 2
                if [ $(($BUTTON_IO)) -gt 0 ]; then
                    button
                    if [ $? -eq 0 ]; then
                        $LCD_APP -i "Shuting down..." > /dev/null
                        echo "shutdown -h now"
                        sudo shutdown -h now
                        exit 0
    fi  fi  fi  fi  fi
}

button
while [ $? -eq 0 ]; do
    echo `date4log` "[Init] Waiting for GPIO Configuring ボタン状態を確認中" >> $LOG 2>&1
    raspi-gpio set ${BUTTON_IO} ip pu
    sleep 1
    button
done

echo `date4log` "[Init] Connecting to LCD 起動中" >> $LOG 2>&1
sleep 0.1
$LCD_APP -i -r${LCD_IO} "LC reset GPIO"${LCD_IO} > /dev/null
sleep 0.1

if [ $(( CPU_CORES )) -eq 0 ]; then
    CPU_CORES=1
fi
echo `date4log` "[Init] CPU_CORES =" $CPU_CORES >> $LOG 2>&1

$LCD_APP -i -b 0 >/dev/null
i=100
while [ $i -gt 4 ]; do
    $LCD_APP -i -n -d $i >/dev/null
    i=$(( i - 6 ))
done
i=4
while [ $i -le 100 ]; do
    $LCD_APP -i -n -b $i >/dev/null
    i=$(( i + 6 ))
done
i=96
while [ $i -ge 0 ]; do
    $LCD_APP -i -n -b $i >/dev/null
    i=$(( i - 6 ))
done

echo `date4log` "[Init] Started Main Loop メイン処理を開始" >> $LOG 2>&1
#wall \($0\) "Started RasPi Monitor"
while true; do
    # 時計
    echo `date4log` "[Loop] Clock 時計表示" >> $LOG 2>&1
    hour=$((`date "+%_I"`))
    hour_pt=$(( ( ( $hour + 6 ) % 12 ) * 100 / 12 ))
    $LCD_APP -i -n -d $hour_pt >/dev/null
    $LCD_APP -y2 `date "+%R:%S"` >/dev/null
    sleep 2
    button_shutdown

    # UPTIME CPU負荷
    echo `date4log` "[Loop] CPU Load 負荷表示" >> $LOG 2>&1
    cpu_f=`uptime|rev|cut -d',' -f3|tr -d ' '|rev|cut -d':' -f2`
    cpu100=`echo "${cpu_f} * 100 / ${CPU_CORES}"|bc`
    cpu_i=`printf "%.0f" $cpu100`
    echo `date4log` "[Value] CPU =" ${cpu_i} >> $LOG 2>&1
    if [ $cpu_i -ge 80 ]; then
        echo `date4log` "[Caution] CPU負荷が高くなっています。現在、"${cpu_i}"％です。" >> $LOG 2>&1
        wall \($0\) "CPU Load =" ${cpu_i}
            /home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
    fi
    if [ $cpu_i -lt 0 ]; then # 負にならないがraspi_lcdが負値に非対応
        cpu_i=0
    fi
    $LCD_APP -i -n -b $cpu_i >/dev/null
    $LCD_APP -y2 "CPU" $cpu_f >/dev/null
    sleep 2
    button_shutdown

    # CPU Temperature
    echo `date4log` "[Loop] CPU Temperature CPU温度表示" >> $LOG 2>&1
    temp=`cat /sys/devices/virtual/thermal/thermal_zone0/temp`  # 温度を取得
    temp_i=$(( $temp / 1000 ))
    echo `date4log` "[Value] TMP =" ${temp_i} >> $LOG 2>&1
    temp_pt=$(( ( $temp - 40000 ) / 400 ))
    if [ $temp_pt -lt 0 ]; then # raspi_lcdが負値に非対応
        temp_pt=0
    fi
    if [ $temp_i -ge 80 ]; then
        echo `date4log` "[Caution] CPU温度が高くなっています。現在、"${temp_i}"℃です。" >> $LOG 2>&1
        wall \($0\) "CPU Temperature =" ${temp_i}
            /home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
    fi
    $LCD_APP -i -n -b $temp_pt >/dev/null
    $LCD_APP -y2 "TMP" ${temp_i}"ﾟC" >/dev/null
    sleep 2
    button_shutdown

    # FREE メモリ容量
    echo `date4log` "[Loop] Used Memory メモリ容量表示" >> $LOG 2>&1
    mem=`free|grep "Mem:"`
    mem_total=$((`echo $mem|awk '{print $2}'`))
    mem_avail=$((`echo $mem|awk '{print $7}'`))
    mem_i=$(( $mem_avail * 100 / $mem_total * -1 + 100 ))
    echo `date4log` "[Value] MEM =" ${mem_i} >> $LOG 2>&1
    if [ $mem_i -lt 0 ]; then # 負にならないがraspi_lcdが負値に非対応
        mem_i=0
    fi
    if [ $mem_i -ge 90 ]; then
        MEMS_sum=`echo ${MEMS[*]}|tr " " "\n"|awk '{sum+=$1} END {print sum}'`
        MEMS_ave=$(( MEMS_sum / ${#MEMS[*]} ))
        if [ $mem_i -gt $MEMS_ave ]; then
            echo `date4log` "[WARNING] メモリ使用量の異常事態です。現在、"${mem_i}"％です。" >> $LOG 2>&1
            wall \($0\) "[WARNING] Used Memory =" ${mem_i}
            /home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
        else
            echo `date4log` "[Caution] メモリ使用量が高くなっています。現在、"${mem_i}"％です" >> $LOG 2>&1
            wall \($0\) "Used Memory =" ${mem_i}
            /home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
        fi
    fi
    if [ $SECONDS -ge 10800 ]; then #3時間経過
        SECONDS=0
        MEMS=(${MEMS[1]} ${MEMS[2]} ${MEMS[3]} ${MEMS[4]} ${MEMS[5]} ${MEMS[6]} ${MEMS[7]} $mem_i)
        # unset MEMS[0]
        # MEMS+=($mem_i)
        MEMS_sum=`echo ${MEMS[*]}|tr " " "\n"|awk '{sum+=$1} END {print sum}'`
        MEMS_ave=$(( MEMS_sum / ${#MEMS[*]} ))
        echo `date4log` "[Calc] Average("${MEMS[*]}")=" $MEMS_ave >> $LOG 2>&1
    fi
    $LCD_APP -i -n -b $mem_i >/dev/null
    $LCD_APP -y2 "MEM" ${mem_i}"%" >/dev/null
    sleep 2
    button_shutdown

    # DF SDカード容量
    echo `date4log` "[Loop] Used SD Card SDカード容量表示" >> $LOG 2>&1
    sdcap=`df | grep "/dev/root"|awk '{print $5}'|cut -d"%" -f1`
    sdcap_i=$(($sdcap))
    echo `date4log` "[Value] SSD =" ${sdcap_i} >> $LOG 2>&1
    if [ $sdcap_i -lt 0 ]; then
        sdcap_i=0
    fi
    if [ $sdcap_i -ge 60 ]; then
        if [ $sdcap_i -ge 90 ]; then
            echo `date4log` "[WARNING] SDカード使用率の異常事態です。現在、"${sdcap_i}"です" >> $LOG 2>&1
            wall \($0\) "[WARNING] Used Memory =" ${mem_i}
            /home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
        else
            echo `date4log` "[Caution] SDカード使用率が"${sdcap_i}"％です" >> $LOG 2>&1
            wall \($0\) "Used Memory =" ${mem_i}
            /home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
        fi
    fi
    $LCD_APP -i -n -b $sdcap_i >/dev/null
    $LCD_APP -i -y2 "SSD" ${sdcap_i}"%" >/dev/null
    sleep 2
    button_shutdown
done
