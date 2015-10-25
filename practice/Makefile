CC = gcc -Wall -O1
PROGS =	practice01 \
		practice02 \
		practice03 \
		practice04 \
		practice05 \
		practice06 \
		practice07 \

all: $(PROGS)
	gcc -Wall -O1 ../gpio/raspi_gpo.c -o raspi_gpo

clean:
	rm -f $(PROGS)
	rm -f raspi_gpo a.out
	rm -f data.csv
