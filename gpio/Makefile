CC = gcc -Wall -O1
PROGS =	raspi_gpi \
		raspi_gpo \
		raspi_ir_in \

all: $(PROGS)
		gcc -Wall -O1 -c ../libs/soft_i2c.c -o ../libs/soft_i2c.o
		gcc -Wall -O1 raspi_i2c.c ../libs/soft_i2c.o -o raspi_i2c
		# ========================================
		# Examples for Raspberry Pi (Raspbian)
		#                         by Wataru KUNINO
		# ========================================

clean:
	rm -f $(PROGS) ../libs/soft_i2c
