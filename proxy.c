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
  //printf("%s\n", bufferLine); //Check/print host - this is just to test.
  
  //After this i'm trying to connect to host, which is in bufferLine. This doesn't work.
  /*
  struct hostent *he;
  if ( (he = gethostbyname(bufferLine) ) == NULL )//This is an outdated function, I should use getaddrinfo() 
  {
      exit(1);
  }
  
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errCheck(sockfd);
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(80);
  memcpy(&servaddr.sin_addr, he->h_addr_list[0], he->h_length);
   
  errcode = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  errCheck(errcode);
  errcode = write(sockfd, buffer, sizeof(buffer));
  errCheck(errcode);
  errcode = read(sockfd, buffer, MAXLEN);
  errCheck(errcode);
  printf("%s\n", buffer);

  errcode = close(sockfd);
  errCheck(sockfd);
  */
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
