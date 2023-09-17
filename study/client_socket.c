#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    /********************************************************************************
    1) socket: create socket
    - 소켓 생성에 성공하면 새로운 소켓의 file descriptor를 반환하고, 실패하면 -1을 반환함
    - 여기에서 반환 받은 file descriptor는 아직 읽고 쓸 준비가 되지 않은 상태임
    */
    int network_socket;
    // AF_INET: 32-bit IP 주소, SOCK_STREAM: 소켓이 연결의 endpoint가 된다는 뜻
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    /********************************************************************************
    2) connect
    - 클라이언트는 connect 함수를 호출하여 서버와 연결
    - 연결에 성공하면 0을 반환, 에러가 발생하면 -1을 반환
    - 연결에 성공하거나 실패할 때까지 connect 함수는 block하고 있음
    - 연결에 성공하면 client descriptor는 읽고 쓸 준비가 완료됨
    */
    // sockaddr_in: specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);       // 포트 번호를 네트워크 바이트 순서로 변환
    server_address.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0

    // sockaddr_in은 IPv4를 위한 구조체라면 sockaddr은 모든 종류의 주소를 처리하기 위한 범용 구조체
    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    // check for error with the connection
    if (connection_status == -1)
    {
        printf("There was an error making a connection to the remote socket\n\n");
    }

    /********************************************************************************
    3) recv: receive data from the server
    */
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);

    // print out the server response
    printf("The server sent the data %s\n", server_response);

    /********************************************************************************
    4) close: Close the socket
    */
    close(network_socket);

    return 0;
}

/*
htons
- htons 함수는 u_short 호스트에서 TCP/IP 네트워크 바이트 순서(big-endian)로 변환
*/