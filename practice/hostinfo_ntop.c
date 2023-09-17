#include "../csapp.h"

/*
Practice Problem 11.4
The getaddrinfo and getnameinfo functions subsume the functionality of inet_pton and inet_ntop, respectively, 
and they provide a higher-level of abstraction that is independent of any particular address format. 
To convince yourself how handy this is, 
write a version of hostinfo (Figure 11.17) that uses inet_ntop instead of getnameinfo 
to convert each socket address to a dotted-decimal address string.
*/

int main(int argc, char **argv)
{
    struct addrinfo *p, *listp, hints;
    struct sockaddr_in *sockp;
    char buf[MAXLINE];
    int rc;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    /* Get a list of addrinfo records */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* Connections only */
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }
    /* Walk the list and display each associated IP address */
    for (p = listp; p; p = p->ai_next)
    {
        sockp = (struct sockaddr_in *)p->ai_addr;
        Inet_ntop(AF_INET, &(sockp->sin_addr), buf, MAXLINE);
        printf("%s\n", buf);
    }
    /* Clean up */
    Freeaddrinfo(listp);
    exit(0);
}