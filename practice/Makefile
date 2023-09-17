CC = gcc
CFLAGS = -O2 -Wall -I .

all: hex2dd dd2hex hostinfo hostinfo_ntop

csapp.o: ../csapp.c ../csapp.h
	$(CC) $(CFLAGS) -c ../csapp.c

hex2dd.o: hex2dd.c ../csapp.h
	$(CC) $(CFLAGS) -c hex2dd.c

hex2dd: hex2dd.o csapp.o

dd2hex.o: dd2hex.c ../csapp.h
	$(CC) $(CFLAGS) -c dd2hex.c

dd2hex: dd2hex.o csapp.o

hostinfo.o: hostinfo.c ../csapp.h
	$(CC) $(CFLAGS) -c hostinfo.c

hostinfo: hostinfo.o csapp.o

hostinfo_ntop.o: hostinfo_ntop.c ../csapp.h
	$(CC) $(CFLAGS) -c hostinfo_ntop.c

hostinfo_ntop: hostinfo_ntop.o csapp.o

clean:
	rm -f *.o hex2dd csapp.o hex2dd.o dd2hex dd2hex.o hostinfo hostinfo.o hostinfo_ntop hostinfo_ntop.o *~