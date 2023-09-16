# Makefile for Proxy Lab 
#
# You may modify this file any way you like (except for the handin
# rule). You instructor will type "make" on your specific Makefile to
# build your proxy from sources.

CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -lpthread

all: proxy echoclient echoserveri

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

proxy.o: proxy.c csapp.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o csapp.o
	$(CC) $(CFLAGS) proxy.o csapp.o -o proxy $(LDFLAGS)


# 🏔 echo
echoclient.o: echoclient.c csapp.h	
echoserveri.o: echoserveri.c csapp.h
echoclient: echoclient.o csapp.o
echoserveri: echoserveri.o csapp.o

# Creates a tarball in ../proxylab-handin.tar that you can then
# hand in. DO NOT MODIFY THIS!
handin:
	(make clean; cd ..; tar cvf $(USER)-proxylab-handin.tar proxylab-handout --exclude tiny --exclude nop-server.py --exclude proxy --exclude driver.sh --exclude port-for-user.pl --exclude free-port.sh --exclude ".*")

clean:
	rm -f *~ *.o proxy core echoclient echoserveri *.tar *.zip *.gzip *.bzip *.gz

