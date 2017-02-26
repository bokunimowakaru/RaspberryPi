CC = gcc -Wall -O1
PROGS =	raspi_gpi \
		raspi_gpo \
		raspi_ir_in \
		raspi_temp \
		raspi_max6675

all: $(PROGS)
		gcc -Wall -O1 -c ../libs/soft_i2c.c -o soft_i2c.o
		gcc -Wall -O1 raspi_lcd.c soft_i2c.o -o raspi_lcd
		gcc -Wall -O1 raspi_bme280.c soft_i2c.o -o raspi_bme280
		gcc -Wall -O1 raspi_hdc1000.c soft_i2c.o -o raspi_hdc1000
		gcc -Wall -O1 raspi_si7021.c soft_i2c.o -o raspi_si7021
		gcc -Wall -O1 raspi_stts751.c soft_i2c.o -o raspi_stts751
		gcc -Wall -O1 raspi_am2320.c soft_i2c.o -o raspi_am2320
		gcc -Wall -O1 raspi_lps25h.c soft_i2c.o -o raspi_lps25h
		gcc -Wall -O1 raspi_ads1115.c soft_i2c.o -o raspi_ads1115
		gcc -Wall -O1 raspi_adxl345.c soft_i2c.o -o raspi_adxl345
		# gcc -Wall -O1 -lwiringPi raspi_ir_out.c  -o raspi_ir_out
		# ========================================
		# Examples for Raspberry Pi (Raspbian)
		#                         by Wataru KUNINO
		# ========================================

clean:
	rm -f $(PROGS) ../libs/soft_i2c
	rm -f raspi_lcd raspi_bme280 raspi_hdc1000 raspi_si7021
	rm -f raspi_stts751 raspi_am2320 raspi_lps25h 
	rm -f raspi_ads1115 raspi_adxl345
	rm -f raspi_ir_out
