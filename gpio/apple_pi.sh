#!/bin/bash
#
# Apple Pi DEMO
#
# Copyright (c) 2017 Wataru KUNINO
#
# /etc/rc.localへ下記を追加すると自動的に起動する
#       /home/pi/RaspberryPi/gpio/apple_pi.sh &

while true; do
/home/pi/RaspberryPi/gpio/raspi_lcd -i `hostname -I|cut -d" " -f1`
/home/pi/RaspberryPi/gpio/raspi_gpo 5 1 >/dev/null
sleep 0
/home/pi/RaspberryPi/gpio/raspi_lcd -i `/home/pi/RaspberryPi/gpio/raspi_bme280`
/home/pi/RaspberryPi/gpio/raspi_gpo 5 0 >/dev/null
/home/pi/RaspberryPi/gpio/raspi_gpo 6 1 >/dev/null
sleep 2 
/home/pi/RaspberryPi/gpio/raspi_lcd -i `date "+%y/%m/%d%R:%S"`
/home/pi/RaspberryPi/gpio/raspi_gpo 6 0 >/dev/null
sleep 2
IN=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`
if [ $IN = "0" ]; then
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "shuting down..."
	sleep 2
	IN=`/home/pi/RaspberryPi/gpio/raspi_gpi 27 PUP`
	if [ $IN = "0" ]; then
		/home/pi/RaspberryPi/gpio/raspi_lcd -i "Bye."
		sudo shutdown -h now
		exit 0
	fi
	/home/pi/RaspberryPi/gpio/raspi_lcd -i "Canceled"
fi
done
