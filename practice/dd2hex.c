#include "../csapp.h"

/*
Practice Problem 11.3

Write a program dd2hex.c that converts its 16-bit network byte order to a 16-bit hex number and prints the result. 
For example,

linux> ./dd2hex 1024 
0x400
*/
int main(int argc, char **argv)
{
    struct in_addr inaddr;
    int rc;

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <network byte order>\n", argv[0]);
        exit(0);
    }

    rc = inet_pton(AF_INET, argv[1], &inaddr); // pointer to network
    if (rc == 0)
        app_error("inet_pton error: invalid network byte order");
    else if (rc < 0)
        unix_error("inet_pton error");

    printf("0x%x\n", ntohs(inaddr.s_addr));
    
    exit(0);
}