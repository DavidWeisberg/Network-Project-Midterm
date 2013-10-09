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
  int listenfd, connectfd, errcode, n, i;
  int loop = 1;
  struct sockaddr_in cliaddr, servaddr;
  socklen_t clilen;
  char buffer[MAXLEN];
  char revbuffer[MAXLEN];

  //SOCKET
  listenfd = socket(AF_INET, SOCK_STREAM, 0);//PF_INET, , IPPROTO_TCP
  errCheck(listenfd);
  
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//Any address
  servaddr.sin_port = htons(0);//Any port
  
  //BIND
  errcode = bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  errCheck(errcode); 
  
  //LISTEN
  errcode = listen(listenfd, 1);//backlog of 1, 0 should default to some set min value. I don't think it matters
  errCheck(errcode);
//Added Code
  int size = sizeof(servaddr);
  errcode = getsockname(listenfd, (struct sockaddr *) &servaddr, &size);
  errCheck(errcode);
  printf("%d\n", ntohs(servaddr.sin_port));
//-Added Code

  //ACCEPT
  clilen = sizeof(cliaddr);
  connectfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
  errCheck(connectfd);
  //getchar(); //System pause used in C   
  /*    
  while(loop)
  { 
    //READ - WRITE
    memset(&buffer, 0, MAXLEN);
    memset(&revbuffer, 0, MAXLEN);
    n = read(connectfd, buffer, MAXLEN);
    if (n < 0)
      perror("Error");
      
    if(buffer[0] == '-' && buffer[1] == '-')//Begin connection close
    {
      for(i = 2; i < MAXLEN; i++)
      {
        if(isprint(buffer[i]))
          break;
      }
      if(i == MAXLEN)
      {
      loop = 0;
      break;
      }
    }
    else if(buffer[0] == '^')//reverse case and remove ^
    {
      for( i = 1; i < strlen(buffer); i++)
      {
        if(islower(buffer[i]))
          buffer[i-1] = toupper(buffer[i]);
        else if(isupper(buffer[i]))
          buffer[i-1] = tolower(buffer[i]);
        else
          buffer[i-1] = buffer[i];//for everything else, spaces, #'s, etc...
      }
      memset(&buffer[strlen(buffer)-1], 0, 1);//gets rid of that last charachter (Probably a terrible way to do this, but WTH it works)
      //buffer[strlen(buffer)-1] = NULL; //This doesn't work, but I think I should be doing something like this
    }
    for( i = 0; i < strlen(buffer); i++)//reverse string
    {
      revbuffer[i] = buffer[strlen(buffer)-1-i];
    }
    n = write(connectfd, revbuffer, MAXLEN);
    if (n < 0)
      perror("Error");
    
  }
*/
    //CLOSE
    errcode = close(connectfd);
    errCheck(errcode);
    errcode = close(listenfd);
    errCheck(errcode);
    return 0; 
}
