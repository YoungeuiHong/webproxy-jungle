#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    char server_message[256] = "You have reached the server.";

    /********************************************************************************
    1) socket: create socket
    - 소켓 생성에 성공하면 새로운 소켓의 file descriptor를 반환하고, 실패하면 -1을 반환함
    */
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    /********************************************************************************
    2) bind
    - The bind function asks the kernel to associate the server’s socket address in addr with the socket descriptor sockfd.
    */

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and Port
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    /********************************************************************************
     3) listen
    - The listen function converts sockfd from an active socket to a listening socket that can accept connection requests from clients.
    - The backlog argument is a hint about the number of outstanding connection requests that the kernel should queue up before it starts to refuse requests.
    */
    listen(server_socket, 5);

    /********************************************************************************
     4) accept
    - int accept(int listenfd, struct sockaddr *addr, int *addrlen);
    - Servers wait for connection requests from clients by calling the accept function. 
    - 서버는 listenfd 파일 디스크립터에 연결 요청이 오면, addr에 클라이언트 소켓 주소를 채워서 connected descriptor를 리턴함
    - 서버는 이 connected descriptor와 Unix I/O function을 사용하여 클라이언트와 통신할 수 있음 
    */
    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    /********************************************************************************
     5) send
    */
    send(client_socket, server_message, sizeof(server_message), 0);

    /********************************************************************************
     6) close
    */
    close(client_socket);
    close(server_socket);

    return 0;
}

/*
🤔 listening descriptor와 connected descriptor는 무슨 차이일까?
- listening descriptor: client connection requests의 endpoint. 한 번 생성되면 서버의 lifetime 동안 유효함
- connected descriptor: 클라이언트의 connection을 서버가 accept할 때마다 생성됨. 서버와 클라이언트가 연결되어 있는 동안만 유효함
*/