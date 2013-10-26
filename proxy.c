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
  struct sockaddr_in cliaddr, servaddr, apacheaddr;
  socklen_t clilen;
  char buffer[MAXLEN];
  char bufferLine[MAXLEN];
  
  //SOCKET
  listenfd = socket(AF_INET, SOCK_STREAM, 0);//PF_INET, , IPPROTO_TCP
  errCheck(listenfd);
  
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//Any address
  servaddr.sin_port = htons(9735);//Any port CHANGE THIS TO 0
  
  //BIND
  errcode = bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  errCheck(errcode); 
  
  //LISTEN
  errcode = listen(listenfd, 1);//backlog of 1, 0 should default to some set min value. I don't think it matters
  errCheck(errcode);
  
//Prints out port number
  int size = sizeof(servaddr);
  errcode = getsockname(listenfd, (struct sockaddr *) &servaddr, &size);
  errCheck(errcode);
  printf("%d\n", ntohs(servaddr.sin_port));


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
    if(buffer[i] == 'H' && buffer[i+1] == 'T' && buffer[i+2] == 'T' && buffer[i+3] == 'P' && buffer[i+4] == '/' && buffer[i+5] == '1') //This is terrible but shows the clients request
      break;
    bufferLine[i] = buffer[i];
  }
  printf("%s:\t%s\n", inet_ntoa(cliaddr.sin_addr), bufferLine);//IP address
  errcode = write(connectfd, buffer, sizeof(buffer));
  errCheck(errcode);
  errcode = close(connectfd);
    errCheck(errcode);
  }
  //getchar(); //System pause used in C
  
  /*
  //THis Doesn't WORK!
  //Socket for forwarding request
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errCheck(sockfd);
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(80);
  errcode = inet_pton(AF_INET, "192.168.70.129", &apacheaddr.sin_addr);
  errCheck(errcode);
  
  //CONNECT - Forward Request to Apache
  errcode = connect(sockfd, (struct sockaddr *) &apacheaddr, sizeof(apacheaddr));
  errCheck(errcode);
  errcode = write(sockfd, buffer, sizeof(buffer));
  errCheck(errcode);
  memset(&buffer, 0, MAXLEN);
  errcode = read(sockfd, buffer, MAXLEN);
  errCheck(errcode);
  errcode = write(connectfd, buffer, sizeof(buffer));
  errCheck(errcode);
 */
    //CLOSE
    //errcode = close(sockfd);
    //errCheck(errcode);
    errcode = close(connectfd);
    errCheck(errcode);
    errcode = close(listenfd);
    errCheck(errcode);
    return 0; 
}
