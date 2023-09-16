#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv)
{
    int listenfd, connfd; // 소켓 디스크립터 => listenfd: 서버 소켓(리스닝 소켓), connfd: 클라이언트와의 실제 통신에 사용되는 연결된 소켓
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; // 클라이언트 주소 정보를 저장하기 위한 구조체
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2) // 인자의 개수가 두 개가 아니면 오류 메세지 출력 후 종료
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]); // 지정된 포트(argv[1])로 들어오는 연결 수락을 위해 Open_listenfd 함수를 호출하여 리스닝 소켓(listenfd) 생성

    while (1)
    {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 클라이언트로부터 연결 요청이 들어오면 신규 연결용 소켓(connfd) 생성
        Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0); // 클라이언트의 정보 출력
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        echo(connfd);
        Close(connfd); // 클라이언트 통신용 소켓 닫기
    }
    exit(0);
}

/*
텍스트 줄을 읽고 echo해주는 echo 함수
- connfd: 소켓 디스크립터
*/
void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio; // buffered reader

    Rio_readinitb(&rio, connfd);                         // 소켓 디스크립터로부터 내용을 읽어옴
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) // EOF를 만날 때까지 반복
    {
        printf("server received %d bytes\n", (int)n);
        Rio_writen(connfd, buf, n);
    }
}