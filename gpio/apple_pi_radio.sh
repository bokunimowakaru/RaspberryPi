#!/bin/bash
#
# Apple Pi DEMO + インターネットラジオ
#
# Copyright (c) 2017 - 2022 Wataru KUNINO
#
# /etc/rc.localへ下記を追加すると自動的に起動する
#       /home/pi/RaspberryPi/gpio/apple_pi_radio.sh &
#
# 実行権限の付与が必要
# chmod u+x /etc/rc.local
#
# ネットラジオ検索
# https://directory.shoutcast.com/

export SDL_AUDIODRIVER=alsa
export AUDIODEV=hw:1,0		# Apple PiのDACに合わせる
RADIO=0
LCD=0
BME=""
TARGET_SEC=$((SECONDS))

while true; do
IN1=`/home/pi/RaspberryPi/gpio/raspi_gpi 22 PUP`
IN2=`/home/pi/RaspberryPi/gpio/raspi_gpi 23 PUP`
IN3=`/home/pi/RaspberryPi/gpio/raspi_gpi 24 PUP`
IN4=`/home/pi/RaspberryPi/gpio/raspi_gpi 25 PUP`
IN5=`/home/pi/RaspberryPi/gpio/raspi_gpi 26 PUP`
IN6=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`

if [ "$IN1" = "0" ]; then
	# LCDに表示                             0123456701234567
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "1:181.fmPow.Exp."
	kill `pidof ffplay` &> /dev/null
	RADIO=1
	ffplay -nodisp http://listen.livestreamingservice.com/181-powerexplicit_64k.aac &> /dev/null &
elif [ "$IN2" = "0" ]; then
	# LCDに表示                             0123456701234567
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "2:181.fmUK Top40"
	kill `pidof ffplay` &> /dev/null
	RADIO=1
	ffplay -nodisp http://listen.livestreamingservice.com/181-uktop40_64k.aac &> /dev/null &
elif [ "$IN3" = "0" ]; then
	# LCDに表示                             0123456701234567
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "3:181.fmThe Beat"
	kill `pidof ffplay` &> /dev/null
	RADIO=1
	ffplay -nodisp http://listen.livestreamingservice.com/181-beat_64k.aac &> /dev/null &
elif [ "$IN4" = "0" ]; then
	# LCDに表示                             0123456701234567
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "4:1.FM  AmTrance"
	kill `pidof ffplay` &> /dev/null
	RADIO=1
	ffplay -nodisp http://185.33.21.111:80/atr_128 &> /dev/null &
elif [ "$IN5" = "0" ]; then
	# LCDに表示                             0123456701234567
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "5:NHK-FMOsaka"
	kill `pidof ffplay` &> /dev/null
	RADIO=5
	ffplay -nodisp https://radio-stream.nhk.jp/hls/live/2023509/nhkradirubkfm/master.m3u8 &> /dev/null &
elif [ "$IN6" = "0" ]; then  # ボタン6 ラジオの停止  長押しでシャットダウン
	RADIO=0
	# LCDに表示                             0123456701234567
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "6:STOP  Long=Off"
	sleep 1
	IN6=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`
	if [ "$IN6" = "0" ]; then
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "shuting down..."
		sleep 2
		IN6=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`
		if [ "$IN6" = "0" ]; then
			/home/pi/RaspberryPi/gpio/raspi_lcd -i "Bye."
			sudo shutdown -h now
			exit 0
		fi
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "Canceled"
	fi
fi

if [ $RADIO -eq 0 ] && [ $SECONDS -gt $TARGET_SEC ]; then
	TARGET_SEC=$((SECONDS + 3))
	if [ $LCD -eq 0 ]; then
		/home/pi/RaspberryPi/gpio/raspi_lcd -i `hostname -I|cut -d" " -f1`
		/home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
		LCD=$(( LCD + 1 ))
		BME=`/home/pi/RaspberryPi/gpio/raspi_bme280`;export BME &
	elif [ $LCD -eq 1 ]; then
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "$BME"
		/home/pi/RaspberryPi/gpio/raspi_gpo 5 0 >/dev/null
		/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
		LCD=$(( LCD + 1 ))
	elif [ $LCD -eq 2 ]; then
		/home/pi/RaspberryPi/gpio/raspi_lcd -i `date "+%y/%m/%d%R:%S"`
		/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
		LCD=0
	fi
fi
done
