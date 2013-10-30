
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
#include <iostream>
#include <string.h>
using namespace std;

#define MAXLEN 5000

struct sockaddr_in webaddr;


void get_info( char* buf, char* cmd, char* host )//Borrowed code
{
   char* iterator;
   char bufSave[MAXLEN];

   strcpy( bufSave, buf );

   iterator = strtok( buf, "\n" );
   strcpy( cmd, iterator );

   //cout << "Cmd is: " << cmd << endl;

   iterator = strtok( NULL, "\n" );

   strcpy( host, iterator );

   iterator = strtok( host, " ");
   iterator = strtok( NULL, "\n" );

   strcpy( host, iterator );
   
   string tempString = host;
   tempString[tempString.length() - 1] = '\0';

   strcpy( host, tempString.c_str() );

   //cout << "Host is: " << host << endl << endl;

   iterator = strtok( NULL, " " );

   strcpy( buf, bufSave );
}

int connect_to_server( char* host )//BORROWED CODE
{
   struct hostent *hp; 
   int sock(0);
   int yes(1);

   hp = gethostbyname( host );

     if( hp == 0 )
   {
      cout << "Error getting host by name for host '" << host << "'" << endl;
           //exit(1);
           return -1;
     }

     sock = socket(AF_INET, SOCK_STREAM, 0);
     if( sock < 0 )
   {
           printf("Error! Socket call failed\n");
           exit(2);
     }

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
   {
            perror("setsockopt");
            exit(1);
        }

     webaddr.sin_family = AF_INET;
     webaddr.sin_addr.s_addr = (*(long *)hp->h_addr);
     webaddr.sin_port = htons(80);

   if( connect(sock, (struct sockaddr *)&webaddr, sizeof(webaddr)) < 0 )
   {
      cerr << "Error connecting..." << endl;
      exit( 1 );
   }
   //cout << "success!!\n";
   return sock;
}




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
  struct sockaddr_in listenaddr, cliaddr;
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
  errcode = getsockname(listenfd, (struct sockaddr *) &listenaddr, (socklen_t *)&size);
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
    //BORROWED CODE
    char host[MAXLEN];
    char cmd[MAXLEN];
    char resp[MAXLEN];
    memset(&host, 0, MAXLEN);
    memset(&cmd, 0, MAXLEN);
    memset(&resp, 0, MAXLEN);
    get_info( buffer, cmd, host );//Borrowed code 
    webfd = connect_to_server( host );
    errcode = write( webfd, buffer, strlen(buffer));
    errCheck(errcode);
    errcode = read( webfd, resp, sizeof(resp));
    errCheck(errcode);
    errcode = write(connectfd, resp, sizeof(resp));
    errCheck(errcode);
    errcode = read( connectfd, resp, sizeof(resp));
    errCheck(errcode);
    
    
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
  */
  errcode = close(webfd);
  errCheck(webfd);
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
