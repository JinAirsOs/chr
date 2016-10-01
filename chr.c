/**
 * Author: PanZhendong
 * Created At: 2016.10.1
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>//function tolower()
#include <netdb.h>//function gethostbyname()

typedef struct{
    char host[64];//ip_addr 127.0.0.1,192.168.1.100
    unsigned int port;//80,8080,3000
    char uri[256];// /,/index.html,/index,/?param=1&param1=test
    char body[512];// param0=test&param1=test1
    unsigned int bodylength;
    char method[12];//GET,POST,DELETE,PUT
    char proto[12];//HTTP/1.1,HTTP/1.0
}http_req;

//get host ip address by host name
int get_host_ip(char *hostname,char *dest);
//tcp connet
int socket_connect(char *host,unsigned int port);
//url parse
http_req parse_url(char *url);
//url http://127.0.0.1:3000/?test=1&test1=2
//todo https://
int http_get(char *url);
int http_post(char *param,char *url);
// ./chr get www.baidu.com
// ./chr post 127.0.0.1:3000
int main(int argc, char *argv[])
{
    if(argc<2){
        printf("wrong argument! number\n");
        exit(1);
    }
    //int len = strlen(argv[1]);
    for(int i=0,len = strlen(argv[1]);i<len;i++)
        argv[1][i] = tolower(argv[1][i]);

    if(strcmp(argv[1],"get")==0){
        if(argc!=3){
            printf("error arguments number!\n");
            exit(1);
        }
        http_get(argv[argc-1]);
    }else if(strcmp(argv[1],"post")==0){
        if(argc<3||argc>4){
            printf("error arguments number!\n");
            exit(1);
        }
        if(argc == 4)
            http_post(argv[2],argv[argc-1]);
        else if(argc == 3)
            http_post("",argv[argc-1]);
    }else{
        printf("no implement!\n");
        exit(1);
    }
    exit(0);
}

//http://127.0.0.1:3000/hh/jj/e?test=1&test1=2
int http_get(char *url)
{
    int sockfd;
    //parse_url must before init rqst.method and rqst.proto
    http_req rqst = parse_url(url);
    strcpy(rqst.method,"GET");
    strcpy(rqst.proto,"HTTP/1.1");

    char req[1024] = {0};
    
    sprintf(req,
    "%s %s %s\r\nHost: %s\r\nUser-Agent: curl/7.49.0\r\nAccept: */*\r\nConnection: close\r\n\r\n",
        rqst.method,
        rqst.uri,
        rqst.proto,
        rqst.host);
    /*printf(req);
    fflush(stdout);*/
    get_host_ip(rqst.host,rqst.host);
    sockfd = socket_connect(rqst.host,rqst.port);

    send(sockfd, req, strlen(req),0);
    char msg[1024] = {0};
    while(recv(sockfd, msg, 1023,0)){
        printf("%s",msg);
        fflush(stdout);
        memset(msg,0,sizeof(msg));
    }
    close(sockfd);
    return 1;
}
//http post request
int http_post(char*param,char *url)
{
    int sockfd;
    //parse_url must before init rqst.method and rqst.proto
    http_req rqst = parse_url(url);

    strcpy(rqst.method,"POST");
    strcpy(rqst.proto,"HTTP/1.1");
    if(strlen(param)>0){
        strcpy(rqst.body,param);
        rqst.bodylength = strlen(param);
    }else
        rqst.bodylength = 0;
    
    char req[1024] = {0};

    sprintf(req,
    "%s %s %s\r\nHost: %s\r\nUser-Agent: curl/7.49.0\r\nAccept: */*\r\nContent-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection: close\r\n\r\n%s",
        rqst.method,
        rqst.uri,
        rqst.proto,
        rqst.host,
        rqst.bodylength,
        rqst.body);

    get_host_ip(rqst.host,rqst.host);
    sockfd = socket_connect(rqst.host,rqst.port);

    send(sockfd, req, strlen(req),0);
    char msg[1024] = {0};
    while(recv(sockfd, msg, 1023,0)){
        printf("%s",msg);
        fflush(stdout);
        memset(msg,0,sizeof(msg));
    }
    close(sockfd);
    return 1;
}
//get host ip by hostname ,ip is stored in dest
int get_host_ip(char *hostname,char *dest)
{
    char   *ptr, **pptr;
    struct hostent *hptr;
    char   str[32]={0};
    ptr = hostname;

    if((hptr = gethostbyname(ptr)) == NULL)
    {
        printf(" gethostbyname error for host:%s\n", ptr);
        exit(0);
    }

    printf("official hostname:%s\n",hptr->h_name);
    for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
        printf("alias:%s\n",*pptr);
    //printf("type:%d\t%d\t%d\n",hptr->h_addrtype,AF_INET,AF_INET6 );
    switch(hptr->h_addrtype)
    {
        case AF_INET:
        case AF_INET6:
            pptr=hptr->h_addr_list;

            for(; *pptr!=NULL; pptr++){
                printf("address:%s\n",
                       inet_ntop(hptr->h_addrtype, (void*)*pptr, str, 32));
                fflush(stdout);
            }
            printf("first address: %s\n",
                       inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
            strcpy(dest,inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
            return 1;
        break;
        default:
            printf("unknown address type\n");
        break;
    }
    exit(0);
}

//tcp connet
int socket_connect(char *host,unsigned int port)
{
    int sockfd;
    int result = 0;
    int len = 0; 
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(host);
    address.sin_port = htons(port);
    len = sizeof(address);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    result = connect(sockfd, (struct sockaddr *)&address, len);
    if (result == -1)
    {
        perror("oops: chr1");
        exit(1);
    }
    return sockfd;
}

http_req parse_url(char *url)
{
    http_req rqst;
    memset(&rqst,0,sizeof(rqst));

    if(strlen(url)<7){//0.0.0.0=7
        fprintf(stderr,"error url");
        exit(1);
    }
    char *p = strstr(url,"http://");
    //todo https://
    if(p!=NULL)
    {
        url+=7;
        p+=7;
    }else//todo error url process eg:ftp:// http:;:// htp:/
        p= url;

    while(*p!=':'&&*p!='\0'&&*p!='/'&&*p!='?')
    {
        p++;
    }
    switch(*p){
        case ':':
        *p++='\0';
        strcpy(rqst.host,url);
        char *q=p;
        while(*p!='\0'&&*p!='/'&&*p!='?')
            ++p;
        if(*p=='\0'){
            rqst.port = (unsigned int)atoi(q);
            strcpy(rqst.uri,"/");
        }
        else if(*p=='/'){
            *p++='\0';
            rqst.port = (unsigned int)atoi(q);
            sprintf(rqst.uri,"/%s",p);
        }else{
            *p++='\0';
            rqst.port=(unsigned int)atoi(q);
            sprintf(rqst.uri,"/?%s",p);
        }
        break;
        case '/':
        *p++='\0';
        strcpy(rqst.host,url);
        rqst.port = 80;
        sprintf(rqst.uri,"/%s",p);
        break;
        case '?':
        *p++='\0';
        strcpy(rqst.host,url);
        rqst.port = 80;
        sprintf(rqst.uri,"/?%s",p);
        break;
        default:// '\0'
        strcpy(rqst.host,url);
        rqst.port = 80;
        strcpy(rqst.uri,"/");
    }
    return rqst;
}
