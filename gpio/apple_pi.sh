#!/bin/bash
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
done
