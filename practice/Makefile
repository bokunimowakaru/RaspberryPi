CC = gcc
CFLAGS = -Wall -O1
PROGS =	practice01 \
		practice02 \
		practice03 \
		practice04 \
		practice05 \
		practice06 \
		practice07 \
		../gpio/raspi_gpo

all: $(PROGS) 
	cp -u ../gpio/raspi_gpo ./

clean:
	rm -f $(PROGS)
	rm -f raspi_gpo a.out ../gpio/raspi_gpo
	rm -f data.csv
