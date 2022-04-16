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
#
# 使用したチャンネル
# https://www.181.fm/index.php?p=mp3links
#	http://listen.livestreamingservice.com/181-power_64k.aac
#	http://listen.livestreamingservice.com/181-powerexplicit_64k.aac
#	http://listen.livestreamingservice.com/181-uktop40_64k.aac
#	http://listen.livestreamingservice.com/181-beat_64k.aac
#
#

export SDL_AUDIODRIVER=alsa
export AUDIODEV=hw:1,0		# Apple PiのDACに合わせる
RADIO=0
LCD=0
BME=""
TARGET_SEC=$((SECONDS))

#    0123456701234567
urls=(
	"1:181.fmPower181 http://listen.livestreamingservice.com/181-power_64k.aac"
	"2:181.fmUK_Top40 http://listen.livestreamingservice.com/181-uktop40_64k.aac"
	"3:181.fmThe_Beat http://listen.livestreamingservice.com/181-beat_64k.aac"
	"4:1.FM__AmTrance http://185.33.21.111:80/atr_128"
	"5:NHK-FMOsaka https://radio-stream.nhk.jp/hls/live/2023509/nhkradirubkfm/master.m3u8"
	"6:181.fmPow[Exp] http://listen.livestreamingservice.com/181-powerexplicit_64k.aac"
)
urln=${#urls[*]}

radio () {
	if [ $1 -ge 1 ] && [ $1 -le $urln ]; then
		ch=(${urls[$(($1 - 1))]})
		/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "`echo ${ch[0]}| tr '_' ' '`"
		/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
		kill `pidof ffplay` &> /dev/null
		/home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
		ffplay -nodisp ${ch[1]} &> /dev/null &
	else
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "ERR num"
	fi
	sleep 1
}

/home/pi/RaspberryPi/gpio/raspi_gpo 5 0 >/dev/null
/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
while true; do
	IN1=`/home/pi/RaspberryPi/gpio/raspi_gpi 22 PUP`
	IN2=`/home/pi/RaspberryPi/gpio/raspi_gpi 23 PUP`
	IN3=`/home/pi/RaspberryPi/gpio/raspi_gpi 24 PUP`
	IN4=`/home/pi/RaspberryPi/gpio/raspi_gpi 25 PUP`
	IN5=`/home/pi/RaspberryPi/gpio/raspi_gpi 26 PUP`
	IN6=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`

	if [ "$IN1" = "0" ]; then  # 181.FM Power 181 (Top 40) の [Clean] と [Explicit] の切り替え
		if [ $RADIO -eq 1 ]; then
			RADIO=6
		else
			RADIO=1
		fi
		radio $RADIO
	elif [ "$IN2" = "0" ]; then
		RADIO=2
		radio $RADIO
	elif [ "$IN3" = "0" ]; then
		RADIO=3
		radio $RADIO
	elif [ "$IN4" = "0" ]; then
		RADIO=4
		radio $RADIO
	elif [ "$IN5" = "0" ]; then
		RADIO=5
		radio $RADIO
	elif [ "$IN6" = "0" ]; then  # ボタン6 ラジオの停止  長押しでシャットダウン
		if [ $RADIO -ne 0 ]; then
			RADIO=0
			kill `pidof ffplay` &> /dev/null
			/home/pi/RaspberryPi/gpio/raspi_gpo 5 0 >/dev/null
			# LCDに表示                             0123456701234567
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
			/home/pi/RaspberryPi/gpio/raspi_lcd -i "6:STOP  Hold=Off"
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
			sleep 1
		else
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
			/home/pi/RaspberryPi/gpio/raspi_lcd -i "shuting down..."
			sleep 3
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
			IN6=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`
			if [ "$IN6" = "0" ]; then
				/home/pi/RaspberryPi/gpio/raspi_lcd -i "Bye."
				sudo shutdown -h now
				exit 0
			fi
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
			/home/pi/RaspberryPi/gpio/raspi_lcd -i "Canceled"
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
		fi
	fi

	if [ $RADIO -eq 0 ] && [ $SECONDS -gt $TARGET_SEC ]; then
		TARGET_SEC=$((SECONDS + 3))
		if [ $LCD -eq 0 ]; then
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
			/home/pi/RaspberryPi/gpio/raspi_lcd -i "Apple PinetRadio"
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
			LCD=$(( LCD + 1 ))
		elif [ $LCD -eq 1 ]; then
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
			/home/pi/RaspberryPi/gpio/raspi_lcd -i `hostname -I|cut -d" " -f1`
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
			LCD=$(( LCD + 1 ))
			BME=`/home/pi/RaspberryPi/gpio/raspi_bme280`;export BME &
		elif [ $LCD -eq 2 ]; then
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
			/home/pi/RaspberryPi/gpio/raspi_lcd -i "$BME"
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
			LCD=$(( LCD + 1 ))
		elif [ $LCD -eq 3 ]; then
			DATE=`date "+%y/%m/%d%R:%S"`
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
			/home/pi/RaspberryPi/gpio/raspi_lcd -i $DATE
			/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
			LCD=0
		fi
	fi
done
