#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

void handle_client_request(int clientfd);
void parse(const char *uri, char *hostname, char *path, char *port);
void build_request_header(rio_t *rio, char *header, char *path);

int main(int argc, char **argv)
{
  int listenfd, connfd;
  struct sockaddr_storage clientaddr;
  socklen_t clientlen;
  char hostname[MAXLINE], port[MAXLINE];

  // Check command line arguments
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  // Listen for connections
  listenfd = Open_listenfd(argv[1]);

  // Execute server loop
  while (1)
  {
    // Accept a connection
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);

    // Conduct a transaction with the client
    handle_client_request(connfd);

    // Close the connection descriptor
    Close(connfd);
  }

  return 0;
}

void handle_client_request(int clientfd)
{
  rio_t rio_client, rio_server; // Robust I/O package
  char request_buf[MAXLINE], header_buf[MAXLINE], response_buf[MAXLINE];
  char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char hostname[MAXLINE], port[MAXLINE], path[MAXLINE];
  int proxyfd;

  Rio_readinitb(&rio_client, clientfd);
  if (Rio_readlineb(&rio_client, request_buf, MAXLINE) <= 0)
  {
    printf("Invalid Request.\n");
    return;
  }
  sscanf(request_buf, "%s %s %s", method, uri, version);
  parse(uri, hostname, path, port);

  build_request_header(&rio_client, header_buf, path);

  // Connect to the remote server and forward request from client
  proxyfd = Open_clientfd(hostname, port);
  Rio_readinitb(&rio_server, proxyfd);
  Rio_writen(proxyfd, header_buf, strlen(header_buf));

  // Forward the response from remote server back to the client
  size_t n;
  while ((n = Rio_readnb(&rio_server, response_buf, MAXLINE)) > 0)
  {
    Rio_writen(clientfd, response_buf, n);
  }
}

/* Parse URI to extract host name, path, and port */
void parse(const char *uri, char *hostname, char *path, char *port)
{
  const char *protocol = "http://";
  const int protocol_len = strlen(protocol);

  const char *host_start = uri + protocol_len;
  const char *host_end = strchr(host_start, '/');

  // hostname & path
  if (host_end == NULL)
  {
    strcpy(hostname, host_start);
    strcpy(path, "/");
  }
  else
  {
    strncpy(hostname, host_start, host_end - host_start);
    hostname[host_end - host_start] = '\0';
    strcpy(path, host_end);
  }

  // port
  char *colon = strchr(hostname, ':');
  if (colon != NULL)
  {
    sscanf(colon + 1, "%[^/]", port);
    *colon = '\0';
  }
  else
  {
    strcpy(port, "80");
  }
}

/* Build HTTP request header based on the client's request */
void build_request_header(rio_t *rio, char *header, char *path)
{
  char buf[MAXLINE];
  // Modify http version from 1.0 to 1.1
  sprintf(header, "GET %s HTTP/1.0\r\n", path);
  // Copy rest of header except above one
  while (Rio_readlineb(rio, buf, MAXLINE) > 2)
  {
    if (strncasecmp(buf, "GET", 3) != 0)
    {
      sprintf(header, "%s%s", header, buf);
    }
  }
  sprintf(header, "%s\r\n", header);
}