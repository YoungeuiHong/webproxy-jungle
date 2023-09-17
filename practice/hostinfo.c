#include "../csapp.h"

/*
💚 HOSTINFO
- domain name에 maping된 IP 주소를 가지고 오는 프로그램
- 예를 들어
    $ ./hostinfo naver.com
    223.130.200.104
    223.130.200.107
    223.130.195.95
    223.130.195.200
*/
int main(int argc, char **argv)
{
    struct addrinfo *p, *listp, hints;
    char buf[MAXLINE];
    char service[MAXLINE];

    int rc, flags;
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }
    /* Get a list of addrinfo records */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* Connections only */
    
    if ((rc = getaddrinfo(argv[1], &service, &hints, &listp)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }
    /* Walk the list and display each IP address */
    flags = NI_NUMERICHOST; /* Display address string instead of domain name */
    for (p = listp; p; p = p->ai_next)
    {
        Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, service, MAXLINE, flags);
        printf("%s\n", buf);
    }
    /* Clean up */
    Freeaddrinfo(listp);
    exit(0);
}