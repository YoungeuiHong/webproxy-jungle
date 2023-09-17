#include "csapp.h"

int main(void)
{
    char *buf, *p;
    char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
    int n1 = 0, n2 = 0, result = 0;
    /* Extract the two arguments */
    if ((buf = getenv("QUERY_STRING")) != NULL)
    {
        sscanf(buf, "first=%d&second=%d", &n1, &n2);
    }
    /* Make the response body */
    sprintf(content, "QUERY_STRING=%s", buf);
    sprintf(content, "<h1>Genius Cat</h1>");
    sprintf(content, "%s<img src=\"../coding_cat.gif\" type=\"image/gif\"/>", content);
    sprintf(content, "%s<h1>The answer is: %d + %d = %d\r\n<h1>",
            content, n1, n2, n1 + n2);
    sprintf(content, "%sThanks for visiting!\r\n", content);
    /* Generate the HTTP response */
    printf("Connection: close\r\n");
    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);
    exit(0);
}