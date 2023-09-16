CC = gcc
CFLAGS = -O2 -Wall -I .

all: tcp_server tcp_client
tcp_server: server_socket.c
	$(CC) $(CFLAGS) -o server_socket server_socket.c
tcp_client: client_socket.c
	$(CC) $(CFLAGS) -o client_socket client_socket.c

clean:
	rm -f *.o server_socket client_socket *~