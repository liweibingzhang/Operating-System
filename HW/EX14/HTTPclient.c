#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( EXIT_FAILURE );
}

// Print a short usage message, then exit.
static void usage() {
  fprintf( stderr, "usage: <host> <port> <path>\n" );
  exit( EXIT_FAILURE );
}

/** Simple program to send a request of http1.0 and report the
    response. */
int main( int argc, char *argv[] ) {
  if ( argc != 4 )
    usage();
  
  char *host = argv[ 1 ];
  char *port = argv[ 2 ];
  char *path = argv[ 3 ];
  
  // For getaddrinfo(), tell the system what kinds of addresses we want
  struct addrinfo addrCriteria = {
    .ai_family = AF_UNSPEC,     // Use either IPV4 or IPV6
    .ai_socktype = SOCK_STREAM, // Use byte stream
    .ai_protocol = IPPROTO_TCP  // Use TCP
  };

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( host, port, &addrCriteria, &servAddr ) != 0 )
    fail( "Can't get address info" );

  // Try to just use the first address we get back, make sure we got one.
  if ( servAddr == NULL )
    fail( "Can't get address" );

  // Make a socket with parameters (e.g., IPV4 vs IPV6) from getaddrinfo()
  int sock = socket( servAddr->ai_family, servAddr->ai_socktype,
                     servAddr->ai_protocol );
  if ( sock < 0 )
    fail( "Can't create socket" );

  // Connect this socket to the server.
  if ( connect( sock, servAddr->ai_addr, servAddr->ai_addrlen ) != 0 )
    fail( "Can't connect to server" );

  // We're done wiht the address info now.
  freeaddrinfo(servAddr);

  // Construct the header for the request.
  char header[1024];
  snprintf(header, sizeof(header),
           "GET %s HTTP/1.0\r\n"
           "Host: %s:%s\r\n"
           "User-Agent: CSC 246 class qjin3@ncsu.edu\r\n"
           "Accept:\r\n\r\n", path, host, port);

  
  // Send the request to the server.
  int bytesSent = send(sock, header, strlen(header), 0);
  if ( bytesSent < 0 )
    fail( "Error sending request to server" );

  // Read back and print out the response.
  char response[1024];
  int totalBytesRead = 0;
  while ( true ) {
    int bytesRead = recv(sock, response + totalBytesRead, sizeof(response) - totalBytesRead - 1, 0);
    if ( bytesRead <= 0 )
      break;
    totalBytesRead += bytesRead;
    response[totalBytesRead] = '\0';
  }
  printf("%s", response);

  // Close the socket connection
  close( sock );

  return 0;
}
