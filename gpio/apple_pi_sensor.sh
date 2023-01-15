#!/bin/bash
#
# Apple Pi DEMO
#
# Copyright (c) 2023 Wataru KUNINO
#
# /etc/rc.localへ下記を追加すると自動的に起動する
#       /home/pi/RaspberryPi/gpio/apple_pi_sensor.sh &
#
# 実行権限の付与が必要
# chmod u+x /etc/rc.local

SECONDS=0
PRES=(1013 1013 1013 1013 1013 1013 1013 1013)

/home/pi/RaspberryPi/gpio/raspi_lcd -i -b 0 >/dev/null
i=4
while [ $i -le 100 ]; do
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -n -b $i >/dev/null
	i=$(( i + 6 ))
done
i=96
while [ $i -ge 0 ]; do
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -n -b $i >/dev/null
	i=$(( i - 6 ))
done

while true; do
	# 時計
	hour=$((`date "+%_I"`))
	hour_pt=$(( ( ( $hour + 6 ) % 12 ) * 100 / 12 ))
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -n -b $hour_pt >/dev/null
	/home/pi/RaspberryPi/gpio/raspi_lcd -y2 `date "+%R:%S"` >/dev/null
	sleep 2

	# BME280センサからの取得
	vals=(`/home/pi/RaspberryPi/gpio/raspi_bme280`)
	temp=`printf "%.0f" ${vals[0]}`
	humi=`printf "%.0f" ${vals[1]}`
	pres=`printf "%.0f" ${vals[2]}`

	# 温度： 20～30℃を棒グラフで表示 30℃以上でLED点灯
	temp_pt=`printf "%.0f" $(( ($temp - 20) * 10 ))`
	if [ $temp_pt -lt 0 ]; then
		temp_pt=0
	fi
	if [ $temp -ge 30 ]; then # 30℃以上の時
		/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
	else
		/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
	fi
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -n -b $temp_pt >/dev/null
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -y2 ${vals[0]} "ﾟC" >/dev/null
	sleep 2

	# 湿度： 0～100％を棒グラフで表示
	if [ $humi -lt 0 ]; then
		humi=0
	fi
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -n -b $humi >/dev/null
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -y2 ${vals[1]} "%" >/dev/null
	sleep 2

	# 気圧： 24時間前からの気圧変化 5ポイント以上の変化でLED点灯
	pres_pt=`printf "%.0f" $(( ( $pres - ${PRES[0]} ) * 10 + 5 ))`
		# pres_pt=`printf "%.0f" $(( $pres - 1000) * 100 / 24 ))`  # 1000～1024 hPa表示
	if [ $pres_pt -lt 0 ]; then
		pres_pt=0
		/home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
	else
		/home/pi/RaspberryPi/gpio/raspi_gpo 5 0 >/dev/null
	fi
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -n -b $pres_pt >/dev/null
	/home/pi/RaspberryPi/gpio/raspi_lcd -i -y2 ${pres} "hPa" >/dev/null
	if [ $SECONDS -ge 10800 ]; then ## 3時間経過
		SECONDS=0
		PRES=(${PRES[1]} ${PRES[2]} ${PRES[3]} ${PRES[4]} ${PRES[5]} ${PRES[6]} ${PRES[7]} $pres)
	fi
	sleep 2

	# シャットダウンボタン
	IN=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`
	if [ "$IN" = "0" ]; then
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "shuting down..." >/dev/null
		sleep 2
		IN=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`
		if [ "$IN" = "0" ]; then
			/home/pi/RaspberryPi/gpio/raspi_lcd -i "Bye." >/dev/null
			sudo shutdown -h now
			exit 0
		fi
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "Canceled" >/dev/null
	fi
done
