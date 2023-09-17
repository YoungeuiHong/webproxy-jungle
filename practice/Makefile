CC = gcc
CFLAGS = -O2 -Wall -I .

all: hex2dd dd2hex

csapp.o: ../csapp.c ../csapp.h
	$(CC) $(CFLAGS) -c ../csapp.c

hex2dd.o: hex2dd.c ../csapp.h
	$(CC) $(CFLAGS) -c hex2dd.c

hex2dd: hex2dd.o csapp.o

dd2hex.o: dd2hex.c ../csapp.h
	$(CC) $(CFLAGS) -c dd2hex.c

dd2hex: dd2hex.o csapp.o

clean:
	rm -f *.o hex2dd csapp.o hex2dd.o dd2hex dd2hex.o *~