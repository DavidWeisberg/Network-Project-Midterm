//This code was found at http://www.blizzhackers.cc/viewtopic.php?t=443832
#include <sys/socket.h>
#include <sys/wait.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <netdb.h>
#include <fcntl.h>
#include <string>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE    102400
#define HTTPPORT    80
#define BROWSERPORT 8100

using namespace std;

struct sockaddr_in server_addr;
struct sockaddr_in from;

int bytesRead(0);

int bind_port()

{
   struct sockaddr_in addr;
   int sock(0);
   int yes(1);


       if( ( sock = socket( AF_INET, SOCK_STREAM, 0) ) <0 ) 
       {    
      cerr << "Error creating socket!" << endl;
      exit( 1 );

       }

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
   {
            perror("setsockopt");
            exit(1);
        }



       memset( &addr, 0, sizeof( addr ) );

       addr.sin_family = AF_INET;

       addr.sin_port = htons( 0 );//Change to 0 for any address

       addr.sin_addr.s_addr = htonl(INADDR_ANY);;

   if( bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0 ) 
       {
      perror("Bind");
      cerr << "Error binding port" << endl;
      exit( 1 );
       }


   if( listen( sock, 1) < 0 ) 
   {
      cerr << "Error calling listen." << endl;
      exit( 1 );

   }

//Print port #
  int size = sizeof(addr);
  if( getsockname(sock, (struct sockaddr *) &addr, (socklen_t *) &size) < 0)
  {
    cerr << "Error getting sock name." << endl;
      exit( 1 );
  }
 
  printf("%d\n", ntohs(addr.sin_port));
  return sock;
}

int modify_header( char* buf )
{
   string tempBuf;
   char* iterator;
   char* tokIterator;
   int totalBytesDeleted(0);
   int lengthToDelete(0);
   int location(0);

   tempBuf = buf;

   while( tempBuf.find("Proxy-Connection: ") != string::npos )
   {
      iterator = strstr( buf, "Proxy-Connection: ");
      tokIterator = strtok( iterator, "\n" );
      lengthToDelete = strlen( tokIterator );
      totalBytesDeleted += lengthToDelete;
      tempBuf.replace( tempBuf.find("Proxy-Connection: "), lengthToDelete + 1, "" );
   }

   while( tempBuf.find("Connection: ") != string::npos )
   {
      iterator = strstr( buf, "Connection: " );
      tokIterator = strtok( iterator, "\n" );
      lengthToDelete = strlen( tokIterator );
      totalBytesDeleted += lengthToDelete;
      tempBuf.replace( tempBuf.find("Connection: "), lengthToDelete + 1, "" );
   }

   location = tempBuf.find("\r\n\r\n");

   if( location == string::npos )
   {
      tempBuf += "Connection: close\n";
   }
   else
   {
      tempBuf.insert( location, "\nConnection: close\n" );
   }

   totalBytesDeleted -= strlen("Connection: close\n");

   strcpy( buf, tempBuf.c_str() );

   return totalBytesDeleted;
}

void read_request( int file_descriptor, char* buf )
{
   long sum(0);

   //cout << "Reading request from client..." << endl << endl;

   if( ( sum = read( file_descriptor, buf, (BUF_SIZE+1) ) ) <= 0 )
   {
      cerr << "Error! Cannot read request socket" << endl;
      exit(1);
   
   }
   else
   {
      sum -= modify_header( buf );
      buf[sum] = '\0';
      //cout << endl << buf << endl;
   }
}

void forward_request( char* buffer, int length, int toBrowserBinding )
{
   int result(0);

   //cout << "Forwarding request to server..." << endl << endl;

   if( (result = write( toBrowserBinding, buffer, length)) <= 0 )
   {
      perror("Write");
      cerr << "Error! Cannot write to socket" << endl;
      exit(1);

   }
   //else
      //cout << buffer << endl << endl;
}

void get_response( char* buf, int serverBinding )
{
   int result(0);
   char contentLength[BUF_SIZE+1];
   int amountOfContentToRead(0);
   char* iterator;
   char content[BUF_SIZE+1];

   //cout << "Fetching response from server..." << endl << endl;

   if( ( result = read( serverBinding, buf, (BUF_SIZE+1) ) ) <= 0 )
   {
      cerr << "Error! Failed to get response!" << endl;
      exit(1);
   }
   else
      bytesRead = result;
      
   //cout << "\n\nHERE\n\n" << buf << endl << endl;

   iterator = strstr( buf, "\r\n\r\n" );

   while( iterator == NULL )
   {
      result = read( serverBinding, buf, (BUF_SIZE+1) );
      bytesRead += result;
      iterator = strstr( buf, "\r\n\r\n" );
   }

   bytesRead -= modify_header( buf );

   string temp = buf;

   int bytesReadTooFar = bytesRead - ( temp.find( "\r\n\r\n" ) + 4 );

   iterator = strstr( buf, "Content-Length: ");

   if( iterator != NULL )
   {
      strcpy( contentLength, iterator );

      iterator = strtok( contentLength, " " );
      iterator = strtok( NULL, "\n" );

      strcpy( contentLength, iterator );

      contentLength[strlen(iterator)-1] = '\0';

      amountOfContentToRead = atoi(contentLength) - bytesReadTooFar;

      while( amountOfContentToRead > 0 )
      {
         result = read( serverBinding, content, (BUF_SIZE+1) );
         bytesRead += result;
         amountOfContentToRead = amountOfContentToRead - result;
         strncat( buf, content, result );
      }
   }

   //cout << buf << endl << endl;
}

void pass_to_client( char* buffer, int file_descriptor )
{
   int result(0);

   //cout << "Read: " << bytesRead << " bytes." << endl;
   //cout << "Passing info back to client..." << endl << endl;
   
   while( bytesRead > 0 )
   {
      if( (result = write( file_descriptor, buffer, bytesRead)) <= 0 )
      {
         perror("Write");
         cerr << "Error! Cannot write to socket" << endl;
         exit(1);

      }
      else
      {
         //cout << "Bytes Written is: " << result << endl;

         bytesRead = bytesRead - result;
      }

   }

   //cout << buffer << endl << endl;
}

void get_info( char* buf, char* cmd, char* host )
{
   char* iterator;
   char bufSave[BUF_SIZE+1];

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

int connect_to_server( char* host )
{
   struct hostent *hp; 
   int sock(0);
   int yes(1);

   hp = gethostbyname( host );

     if( hp == 0 )
   {
      cout << "Error getting host by name for host '" << host << "'" << endl;
           exit(1);
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

     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = (*(long *)hp->h_addr);
     server_addr.sin_port = htons(80);

   if( connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
   {
      cerr << "Error connecting..." << endl;
      exit( 1 );
   }

   return sock;
}

int handle_data( int file_descriptor, struct sockaddr_in *from )
{
   char buf[BUF_SIZE+1];
   char resp[BUF_SIZE+1];
   int serverBinding(0);
   char cmd[1024];
   char host[1024];
   struct sockaddr_in  server_addr; 

   //Read an HTTP request from our socket;
   //Storing the request inside of buf.
   read_request( file_descriptor, buf );
   
   //David
   //Display request line
      char bufferLine[BUF_SIZE];
      memset(&bufferLine, 0, BUF_SIZE);
      for(int i=0; i < BUF_SIZE+1; i++)
    {
      if(buf[i] == 'H' && buf[i+1] == 'T' && buf[i+2] == 'T' && buf[i+3] == 'P' && 
      buf[i+4] == '/' && buf[i+5] == '1') //This is terrible but shows the clients request
        break;
      bufferLine[i] = buf[i];
    }
    printf("%s:\t%s\n", inet_ntoa(from->sin_addr), bufferLine);//IP address
    //Weisberg
   

   get_info( buf, cmd, host );

   //Form a connection with server....
   serverBinding = connect_to_server( host );

   //cout << "Successfully created a connection to port " << HTTPPORT << " with binding " << serverBinding << endl;

   //Pass Modified Version to Server
   forward_request( buf, strlen( buf ), serverBinding );

   //Get Response
   get_response( resp, serverBinding );

   pass_to_client( resp, file_descriptor );

   shutdown( serverBinding, SHUT_RDWR );
   close( serverBinding );


       shutdown( file_descriptor, SHUT_RDWR );

       close( file_descriptor );

   return 0;
}

void ReleaseChild( int unused )

{

    while(wait3(NULL, WNOHANG, (struct rusage*) 0) >= 0 )

        ;

}

int main()
{
   int file_descriptor(0);
   int child_pid(0);
   socklen_t length;
   int browserBinding(0);

   memset( &from, 0, sizeof( from ) );

   browserBinding = bind_port();

   //Wait for the process to die before starting again!
   signal(SIGCHLD, ReleaseChild);

   //cout << "Waiting for connections..." << endl;

   while( true )
   {
      length = sizeof( from );
      if( ( file_descriptor = accept( browserBinding, (struct sockaddr*)&from, &length ) ) < 0 )
      {
         cerr << "Error! Failed to complete accept command." << endl;
         exit(1);
      }

      //cout << "Found new connection with binding: " << browserBinding << " and file descriptor: " << file_descriptor << endl;

      if( ( child_pid = fork() ) == 0 ) //Here we see that child_pid is returned by fork for the parent process, and 0 is returneded by the child process to indicate success.
      {
         handle_data( file_descriptor, &from ); //The relevant info we need: The socket we're using and who's on the other end of the socket.

         //cout << "Disconnecting from " << getpid() << endl;
         exit( 0 ); //Successful operation.      
      }
      else
      {
         //We failed to fork something properly...
         close( file_descriptor );
      }
   }

   return 0;
}

