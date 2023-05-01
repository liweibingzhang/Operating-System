// Wait for a TCP connection from one client, then send them 1 MB over
// the socket.

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>  // For MIN

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

int main( int argc, char *argv[] ) {
  // For getadderinfo(), prepare a description of server address criteria.
  struct addrinfo addrCriteria = {
    .ai_flags = AI_PASSIVE,     // Plan to accept connections
    .ai_family = AF_UNSPEC,     // Use either IPV4 or IPV6
    .ai_socktype = SOCK_STREAM, // Use byte stream
    .ai_protocol = IPPROTO_TCP  // Use 
  };

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( NULL, PORT_NUMBER, &addrCriteria, &servAddr) )
    fail( "Can't get address info" );

  // Try to just use the first address we get back, make sure we got one.
  if ( servAddr == NULL )
    fail( "Can't get address" );

  // Create a TCP socket, using values we got from getaddrinfo()
  int servSock = socket( servAddr->ai_family, servAddr->ai_socktype,
                         servAddr->ai_protocol);
  if ( servSock < 0 )
    fail( "Can't create socket" );

  // Bind the socket to the local address we got from getaddrinfo()
  if ( bind( servSock, servAddr->ai_addr, servAddr->ai_addrlen ) != 0 )
    fail( "Can't bind socket" );
  
  // Free address list allocated by getaddrinfo()
  freeaddrinfo( servAddr );

  // Tell the socket to listen for incoming connections.
  if ( listen( servSock, 5 ) != 0 )
    fail( "Can't listen on socket" );

  // Wait for a client to connect
  struct sockaddr_storage clntAddr; // Client address
  // Set length of client address structure (in-out parameter)
  socklen_t clntAddrLen = sizeof(clntAddr);

  int sock = accept( servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
  if ( sock < 0 )
    fail( "Can't accept on socket" );

  // Keep sending until we send all the expected data.
  int count = 0;
  while ( count < TOTAL ) {
    // Send a buffer full of data.  It's uninitialized; we don't really
    // care what bytes we're sending.
    char buffer[ BLOCK_SIZE ];
    int n = write( sock, buffer, MIN( BLOCK_SIZE, TOTAL - count ) );

    // Report how much we sent and add it to the total
    printf( "Sent: %d\n", n );
    count += n;
  }

  // Close our connected socket and the socket we were listening with.
  close( sock );
  close( servSock );
  
  return 0;
}
