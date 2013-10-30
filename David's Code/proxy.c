//Borrowed but unused code taken from http://martinbroadhurst.com/source/tcpproxy.c.html
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#define MAXLEN 5000

/*
unsigned int transfer(int from, int to)//Borrowed code
{
    char buf[MAXLEN];
    unsigned int disconnected = 0;
    size_t bytes_read, bytes_written;
    bytes_read = read(from, buf, MAXLEN);
    printf("%s\n", buf);
    if (bytes_read == 0) {
        disconnected = 1;
    }
    else {
        bytes_written = write(to, buf, bytes_read);
        if (bytes_written == -1) {
            disconnected = 1;
        }
    }
    return disconnected;
}

void handle(int client, char *host, char *port)//Borrowed code
{
    struct addrinfo hints, *res;
    int server = -1;
    unsigned int disconnected = 0;
    fd_set set;
    unsigned int max_sock;
    // Get the address info 
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo("www.google.com", "80", &hints, &res) != 0) {
        perror("getaddrinfo");
        close(client);
        return;
    }

    // Create the socket 
    server = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (server == -1) {
        perror("socket");
        close(client);
        return;
    }

    // Connect to the host 
    if (connect(server, res->ai_addr, res->ai_addrlen) == -1) {
        perror("connect");
        close(client);
        return;
    }

    if (client > server) {
        max_sock = client;
    }
    else {
        max_sock = server;
    }

    // Main transfer loop 
    while (!disconnected) {
        FD_ZERO(&set);
        FD_SET(client, &set);
        FD_SET(server, &set);
        if (select(max_sock + 1, &set, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }
        if (FD_ISSET(client, &set)) {
            disconnected = transfer(client, server);
        }
        if (FD_ISSET(server, &set)) {
            disconnected = transfer(server, client);
        }
    }
    close(server);
    close(client);
}
*/




void errCheck(int err)
{
  if(err == -1)
  {
    perror("Error");
    exit(1);  
  }
}

int main()
{
  int listenfd, connectfd, webfd, errcode, n, i;
  struct sockaddr_in listenaddr, cliaddr, webaddr;
  socklen_t clilen;
  char buffer[MAXLEN];
  char bufferLine[MAXLEN];
  
  //SOCKET
  listenfd = socket(AF_INET, SOCK_STREAM, 0);//PF_INET, , IPPROTO_TCP
  errCheck(listenfd);
  
  listenaddr.sin_family = AF_INET;
  listenaddr.sin_addr.s_addr = htonl(INADDR_ANY);//Any address
  listenaddr.sin_port = htons(9735);//Any port CHANGE THIS TO 0
  
  //BIND
  errcode = bind(listenfd, (struct sockaddr *) &listenaddr, sizeof(listenaddr));
  errCheck(errcode); 
  
  //LISTEN
  errcode = listen(listenfd, 1);//backlog of 1, 0 should default to some set min value. I don't think it matters
  errCheck(errcode);
  
//Prints out port number
  int size = sizeof(listenaddr);
  errcode = getsockname(listenfd, (struct sockaddr *) &listenaddr, &size);
  errCheck(errcode);
  printf("%d\n", ntohs(listenaddr.sin_port));


  while(1)
  {
    //ACCEPT
    clilen = sizeof(cliaddr);
    connectfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
    errCheck(connectfd);
    
    //Display Request - WIP
    memset(&buffer, 0, MAXLEN);
    memset(&bufferLine, 0, MAXLEN);
    errcode = read(connectfd, buffer, MAXLEN);
    errCheck(errcode);
    for(i=0; i < MAXLEN; i++)
    {
      if(buffer[i] == 'H' && buffer[i+1] == 'T' && buffer[i+2] == 'T' && buffer[i+3] == 'P' && 
      buffer[i+4] == '/' && buffer[i+5] == '1') //This is terrible but shows the clients request
        break;
      bufferLine[i] = buffer[i];
    }
    printf("%s:\t%s\n", inet_ntoa(cliaddr.sin_addr), bufferLine);//IP address
    
    /*//My crappy way of trying to get host
    memset(&bufferLine, 0, MAXLEN);
    for(i=0; i < MAXLEN; i++)
    {
      if( buffer[i] == '\n' && buffer[i+1] == 'H' && buffer[i+2] == 'o' && buffer[i+3] == 's' && 
      buffer[i+4] == 't' && buffer[i+5] == ':')//Terribly done
      {
        int j;
        int k = 0;
        for(j = i+7; buffer[j] != '\n'; j++)
        {
          bufferLine[k] = buffer[j];
          k++;
        }
      }
    }
*/
    
    
  //printf("%s", bufferLine); //Check/print host - this is just to test.
  
  //webfd = socket(AF_INET, SOCK_STREAM, 0);
  //errCheck(webfd); 
  
  /*//Trying getaddrinfo also doesn't work...
  struct addrinfo hints, *res;
  struct sockaddr_in *ipv4Addr;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;//IPv4 & IPv6
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  errcode = getaddrinfo(bufferLine, NULL, &hints, &res);
  if(errcode != 0)
  {
    perror("Error");
    exit(errcode);
  }
  
  ipv4Addr = (struct sockaddr_in *) res->ai_addr;
  //inet_ntop(res->ai_family, &ipv4Addr->sin_addr, addrstr, INET6_ADDRSTRLEN);
  webaddr.sin_addr.s_addr = ipv4Addr->sin_addr.s_addr;
  */
  
  //After this i'm trying to connect to host, which is in bufferLine. This doesn't work.
/*
  struct hostent *he;
  if ( (he = gethostbyname(bufferLine) ) == NULL )//This is an outdated function, I should use getaddrinfo() 
  {
      printf("0.1\n"); // Problem is getting hostname
      exit(1);
  } 

  webaddr.sin_family = AF_INET;
  webaddr.sin_port = htons(80);
  webaddr.sin_addr.s_addr = (*(long *)he->h_addr);
  */
  /*
  printf("1\n");
  errcode = connect(webfd, (struct sockaddr *) &webaddr, sizeof(webaddr));
  errCheck(errcode);
  printf("2\n");
  errcode = write(webfd, buffer, sizeof(buffer));
  errCheck(errcode);
  printf("3\n");
  errcode = read(webfd, buffer, MAXLEN);
  errCheck(errcode);
  printf("%s\n\n", buffer);
  printf("LOOK UP\n");
  
  errcode = close(webfd);
  errCheck(webfd);
  */
  errcode = close(connectfd);
  errCheck(errcode);
  }

    //CLOSE
    errcode = close(webfd);
    errCheck(errcode);
    errcode = close(connectfd);
    errCheck(errcode);
    errcode = close(listenfd);
    errCheck(errcode);
    return 0; 
}
