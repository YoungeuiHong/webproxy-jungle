#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    // create socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);       // 포트 번호를 네트워크 바이트 순서로 변환
    server_address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address)); // 오류가 발생하지 않으면 0을 반환

    // check for error with the connection
    if (connection_status == -1)
    {
        printf("There was an error making a connection to the remote socket\n\n");
    }

    // receive data from the server
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    // print out the server response
    printf("The server sent the data %s\n", server_response);

    // Close the socket
    close(network_socket);

    return 0;
}

/*
htons
- htons 함수는 u_short 호스트에서 TCP/IP 네트워크 바이트 순서(big-endian)로 변환
*/