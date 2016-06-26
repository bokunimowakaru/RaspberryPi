CC = gcc -Wall -O1
PROGS =	raspi_gpi \
		raspi_gpo \
		raspi_ir_in \

all: $(PROGS)
		gcc -Wall -O1 -c ../libs/soft_i2c.c -o ../libs/soft_i2c.o
		gcc -Wall -O1 raspi_lcd.c ../libs/soft_i2c.o -o raspi_lcd
		gcc -Wall -O1 raspi_stts751.c ../libs/soft_i2c.o -o raspi_stts751
		gcc -Wall -O1 raspi_am2320.c ../libs/soft_i2c.o -o raspi_am2320
		# ========================================
		# Examples for Raspberry Pi (Raspbian)
		#                         by Wataru KUNINO
		# ========================================

clean:
	rm -f $(PROGS) ../libs/soft_i2c
