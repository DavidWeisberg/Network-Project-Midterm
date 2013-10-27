#include "header.h"

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
  int listenfd, connectfd, sockfd, errcode, n, i;
  int loop = 1;
  struct sockaddr_in cliaddr, listenaddr, servaddr;
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


  while(loop)
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
  printf("%s", bufferLine); //Check/print host - this is just to test.
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errCheck(sockfd); 
  
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
  servaddr.sin_addr.s_addr = ipv4Addr->sin_addr.s_addr;
  */
  
  //After this i'm trying to connect to host, which is in bufferLine. This doesn't work.

  struct hostent *he;
  if ( (he = gethostbyname(bufferLine) ) == NULL )//This is an outdated function, I should use getaddrinfo() 
  {
      printf("0.1\n"); // Problem is getting hostname
      exit(1);
  } 

  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(80);
  servaddr.sin_addr.s_addr = (*(long *)he->h_addr);
  
  printf("1\n");
  errcode = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  errCheck(errcode);
  printf("2\n");
  errcode = write(sockfd, buffer, sizeof(buffer));
  errCheck(errcode);
  printf("3\n");
  errcode = read(sockfd, buffer, MAXLEN);
  errCheck(errcode);
  printf("%s\n\n", buffer);
  printf("LOOK UP\n");

  errcode = close(sockfd);
  errCheck(sockfd);
  
  errcode = close(connectfd);
  errCheck(errcode);
  }

    //CLOSE
    errcode = close(sockfd);
    errCheck(errcode);
    errcode = close(connectfd);
    errCheck(errcode);
    errcode = close(listenfd);
    errCheck(errcode);
    return 0; 
}
