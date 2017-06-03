#!/bin/bash
# BLEビーコンのブロードキャストを受信する
# hcidumpのインストール（sudo apt-get install bluez-hcidump）が必要です
#
# ./ble_logger.sh           簡易表示モード
# ./ble_logger.sh dump      受信データ表示モード
# ./ble_logger.sh ana       解析表示モード
# ./ble_logger.sh help      ヘルプ表示
#
# Copyright (c) 2017 Wataru KUNINO

if [ $# -ne 0 -a "${1}" != "ana" -a "${1}" != "dump" ]; then
    echo "Usage: "${0}" [options]"
    head -10 $0|tail -9
    exit 0
fi

interrupted(){
    PID=`sudo pidof hcitool`
    if [ $PID ]; then
        sudo kill $PID &> /dev/null
    fi
    exit 0
}
trap interrupted SIGINT

PID=`sudo pidof hcitool`
if [ -e $PID ]; then
    echo "BLE Logger"
    sudo hciconfig hci0 up
    echo "start LE scan"
    sudo hcitool lescan --pa --du &> /dev/null &
    sleep 1
    if [ "${1}" == "dump" ]; then
        sudo hcidump -R | ./$0 $1
    else
        sudo hcidump | ./$0 $1
    fi
    exit 0
fi

echo "start HCI dump"
while read BLE; do
    STARTFLAG=`echo -E $BLE|cut -d" " -f1`
    DATE=""
    if [ $STARTFLAG == ">" ]; then
        echo
        if [ "${1}" == "ana" ]; then
            BLE=`echo -E $BLE|cut -d" " -f2-`
        elif [ "${1}" == "dump" ]; then
            BLE=`echo -E $BLE|cut -d" " -f2-|tr " " ","`
        else
            BLE=`echo $BLE|cut -d"(" -f2|cut -d")" -f1`
        fi
        DATE=`date "+%Y/%m/%d %R"`
    elif [ "${1}" != "ana" ]; then
        if [ $STARTFLAG == "bdaddr" ]; then
            BLE=`echo $BLE|cut -d" " -f2`
        elif [ $STARTFLAG == "Flags:" ]; then
            BLE=`echo $BLE|cut -d" " -f2`
        elif [ $STARTFLAG == "Complete" ]; then
            BLE=`echo $BLE|cut -d"'" -f2`
        elif [ $STARTFLAG == "Unknown" ]; then
            BLE=`echo $BLE|cut -d" " -f3`
        elif [ $STARTFLAG == "RSSI:" ]; then
            BLE=`echo $BLE|cut -d" " -f2`
        else
            BLE=`echo $BLE|cut -d" " -f1`
        fi
    elif [ "${1}" == "dump" ]; then
        BLE=`echo -E $BLE|tr " " ","`
    fi
    echo -n -E $DATE, $BLE
done
wait `pidof hcitool`
exit
