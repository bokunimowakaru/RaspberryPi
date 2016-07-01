CC = gcc -Wall -O1
PROGS =	raspi_gpi \
		raspi_gpo \
		raspi_ir_in \
		raspi_temp \

all: $(PROGS)
		gcc -Wall -O1 -c ../libs/soft_i2c.c -o soft_i2c.o
		gcc -Wall -O1 raspi_lcd.c soft_i2c.o -o raspi_lcd
		gcc -Wall -O1 raspi_bme280.c soft_i2c.o -o raspi_bme280
		gcc -Wall -O1 raspi_stts751.c soft_i2c.o -o raspi_stts751
		gcc -Wall -O1 raspi_am2320.c soft_i2c.o -o raspi_am2320
		gcc -Wall -O1 raspi_lps25h.c soft_i2c.o -o raspi_lps25h
		# ========================================
		# Examples for Raspberry Pi (Raspbian)
		#                         by Wataru KUNINO
		# ========================================

clean:
	rm -f $(PROGS) ../libs/soft_i2c
