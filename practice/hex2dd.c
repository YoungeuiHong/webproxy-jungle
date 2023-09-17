#include "../csapp.h"


/*
Practice Problem 11.2
Write a program hex2dd.c that converts its 16-bit hex argument to a 16-bit network byte order and prints the result. 
For example

linux> ./hex2dd 0x400 
1024
*/
int main(int argc, char **argv)
{
    struct in_addr inaddr; // Address in network byte order
    uint16_t addr;         // Address in host byte order
    char buf[16];      // Buffer for dotted-decimal string

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <hex number>\n", argv[0]);
        exit(0);
    }

    sscanf(argv[1], "%x", &addr);
    inaddr.s_addr = htons(addr); // host byte order형의 IPv4 주소를 network byte order형으로 변환 (htons: 16비트 / htonl: 32비트)

    if (!inet_ntop(AF_INET, &inaddr, buf, 16)) // IPv4 또는 IPv6 인터넷 네트워크 주소를 인터넷 표준 형식의 문자열로 변환 (network to pointer)
        unix_error("inet_ntop");

    printf("%s\n", buf);

    exit(0);
}

/*
 * exercise 11.2
 */

// #include <stdio.h>
// #include <stdlib.h>
// #include <arpa/inet.h>

// int main(int argc, char *argv[])
// {
// 	if (argc < 2) {
// 		fprintf(stderr, "Need at least 2 command line arguments.\n");
// 		exit(1);
// 	}
	
// 	unsigned n = htonl(strtol(argv[1], NULL, 16));
// 	char *p = (char *) malloc(16);
// 	inet_ntop(AF_INET, &n, p, 16);
// 	printf("%s\n", p);
	
// 	exit(0);
// }