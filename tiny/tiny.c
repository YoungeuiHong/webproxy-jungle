/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize, char *method);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs, char *method);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv)
{
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  // 듣기 소켓을 오픈
  listenfd = Open_listenfd(argv[1]);

  // 무한 서버 루프 실행
  while (1)
  {
    clientlen = sizeof(clientaddr);
    // 연결 요청 접수
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    // 트랜잭션 수행
    doit(connfd); // line:netp:tiny:doit
    // 자신쪽의 연결 끝을 닫기
    Close(connfd); // line:netp:tiny:close
  }
}

/* 한 개의 HTTP 트랜잭션을 처리하는 함수 */
void doit(int fd)
{
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  Rio_readinitb(&rio, fd);

  // 요청 라인을 읽고 분석
  Rio_readlineb(&rio, buf, MAXLINE);
  printf("Request headers: \n");
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version);

  // Tiny 프로그램은 GET 메소드만 지원함
  // 클라이언트가 POST 같은 다른 메소드를 요청하면 에러 메세지를 보내고 main 루틴으로 돌아옴.
  // 그 후에 연결을 닫고 다음 요청을 기다림
  if (!(strcasecmp(method, "GET") == 0 || strcasecmp(method, "HEAD") == 0))
  {
    clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
    return;
  }

  // 읽어들이고 다른 요청 헤더들을 무시함
  read_requesthdrs(&rio);

  // URI를 파일 이름과 비어 있을 수도 있는 CGI 인자 스트링으로 분석하고,
  // 요청이 정적 컨텐츠를 위한 것인지 또는 동적 컨텐츠를 위한 것인지 나타나는 플래그를 설정
  is_static = parse_uri(uri, filename, cgiargs);

  // 만약 이 파일이 디스크 상에 있지 않으면, 에러 메세지를 즉시 클라이언트에게 보내고 리턴
  if (stat(filename, &sbuf) < 0)
  {
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
    return;
  }

  if (is_static)
  { // 정적 컨텐츠 요청이라면
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
    { // regular 파일이 아니거나, 읽기 권한이 없다면
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return;
    }
    serve_static(fd, filename, sbuf.st_size, method);
  }
  else
  { // 동적 컨텐츠 요청이라면
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
    { // 파일 실행이 불가능하다면
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs, method);
  }
}

/* 클라이언트에게 HTML 포맷으로 에러 정보를 반환하는 함수 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF];

  // HTTP response body 만들기
  sprintf(body, "<html><title>Tiny Error</title");
  sprintf(body, "%s<body bgcolor="
                "ffffff"
                ">\r\n",
          body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web Server</em>\r\n", body);

  // HTTP 응답 출력하기
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

/* 요청 헤더를 읽는 함수 (Tiny 서버는 헤더에 포함되어 있는 정보를 사용하지 않으므로, 내용을 읽고 무시함)*/
void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  while (strcmp(buf, "\r\n")) // 비어있는 줄을 만나면 요청 헤더의 끝임을 인식
  {
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}

/* URI를 파싱해서 파일 이름과 CGI 인자를 추출하는 함수 */
int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr;

  if (!strstr(uri, "cgi-bin")) // 주어진 URI에 "cgi-bin"이 포함되어 있지 않은 경우 정적 컨텐츠 요청이라면
  {
    // 정적 컨텐츠 요청이라면 CGI argument string을 지움
    strcpy(cgiargs, "");
    // URI를 Linux 상대 경로로 변환 (ex: ./index.html)
    strcpy(filename, ".");
    strcat(filename, uri);
    // URI가 "/"로 끝나면 디폴트 파일 사용
    if (uri[strlen(uri) - 1] == '/')
      strcat(filename, "home.html");
    // 1을 반환하여 정적 컨텐츠 요청임을 나타냄
    return 1;
  }
  else // 동적 컨텐츠 요청이라면. executable 파일은 ./cgi-bin 디렉토리에 있음
  {
    // CGI argument가 있는 경우 추출
    ptr = index(uri, '?');
    if (ptr)
    {
      strcpy(cgiargs, ptr + 1); // ? 뒤에 있는 CGI 인자를 cgiargs에 저장
      *ptr = '\0';              // ?를 null 종료 문자로 바꿔서 기존 URI에서 "?" 이후 부분을 잘라냄
    }
    else
    {
      strcpy(cgiargs, ".");
    }
    // 나머지 URI를 Linux 상대 경로로 변환
    strcpy(filename, ".");
    strcat(filename, uri);
    // 0을 반환하여 동적 컨텐츠 요청임을 나타냄
    return 0;
  }
}

/*
클라이언트의 요청에 대한 응답으로 정적 파일을 전송하는 과정 수행
- fd: 소켓 파일 디스크립터
- filename: 전송할 정적 파일의 이름
- filesize: 전송할 파일의 크기
*/
void serve_static(int fd, char *filename, int filesize, char *method)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  // 클라이언트에게 response header 전송
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sConnection: close\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf));
  printf("Response headers: \n");
  printf("%s", buf);

  if (strcasecmp(method, "HEAD") == 0)
    return;

  // 클라이언트에서 response body 전송
  srcfd = Open(filename, O_RDONLY, 0);                        // 요청한 파일을 열고, 해당 파일의 디스크립터를 얻음
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); // 해당 파일의 데이터를 가상 메모리로 매핑하고, 매핑된 메모리 영역의 시작 주소를 받아옴
  Close(srcfd);                                               // 파일 디스크립터 닫기
  Rio_writen(fd, srcp, filesize);                             // 데이터를 클라이언트로 전송
  Munmap(srcp, filesize);                                     // 사용한 메모리 매핑 영역 해제

  // Homework Problem 11.9.
  // 만약 위 코드를 malloc, rio_readn, rio_written을 사용하는 것으로 수정한다면
  // srcfd = Open(filename, O_RDONLY, 0);  // 요청한 파일을 열고, 해당 파일의 디스크립터를 얻음
  // srcp = Malloc(filesize);              // 해당 파일의 데이터 크기만큼 메모리를 할당 받음
  // Rio_readn(srcfd, srcp, filesize);     // 파일 디스크립터의 내용을 srcp에 작성
  // Close(srcfd);                         // 파일 디스크립터 닫기
  // Rio_writen(fd, srcp, filesize);    // 데이터를 클라이언트로 전송
  // free(srcp);                           // 할당 받은 메모리 해제
}

/* 파일 이름 바탕으로 파일 타입 파악하기 */
void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".mp4"))
    strcpy(filetype, "video/mp4");
  else
    strcpy(filetype, "text/plain");
}

void serve_dynamic(int fd, char *filename, char *cgiargs, char *method)
{
  char buf[MAXLINE], *emptylist[] = {NULL};

  // HTTP 응답의 첫 부분 리턴
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  if (strcasecmp(method, "HEAD") == 0)
    return;

  if (Fork() == 0)
  {
    setenv("QUERY_STRING", cgiargs, 1);
    // CGI 프로그램은 자신의 동적 컨텐츠를 표준출력을 보낸다.
    // 자식 프로세스가 CGI 프로그램을 로드하고 실행하기 전에 리눅스 dup2함수를 사용해서 표준 출력을 클라이언트와 연결된 connection descriptor로 재저징한다.
    // 따라서 CGI 프로그램이 표준 출력으로 쓰는 모든 것은 클라이언트로 직접 가게 된다.
    Dup2(fd, STDOUT_FILENO);
    Execve(filename, emptylist, environ);
  }

  Wait(NULL);
}