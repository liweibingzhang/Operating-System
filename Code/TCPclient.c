#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/param.h>  // For MIN

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

/** Simple program to send a bunch of data to a TCP server. */
int main( int argc, char *argv[] ) {
  // Make sure we have an argument.
  if ( argc != 2 )
    fail( "usage: TCPclient <server-host>" );
  
  // For getaddrinfo(), tell the system what kinds of addresses we want
  struct addrinfo addrCriteria = {
    .ai_family = AF_UNSPEC,     // Use either IPV4 or IPV6
    .ai_socktype = SOCK_STREAM, // Use byte stream
    .ai_protocol = IPPROTO_TCP  // Use TCP
  };

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if ( getaddrinfo( argv[ 1 ], PORT_NUMBER, &addrCriteria, &servAddr ) != 0 )
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

  // We're done with the address info now.
  freeaddrinfo(servAddr);

  // Keep writing to the socket until we send all the data we're trying to.
  int count = 0;
  while ( count < TOTAL ) {
    // Try to read up to a buffer full of data.
    char buffer[ BLOCK_SIZE ];
    int n = read( sock, buffer, MIN( BLOCK_SIZE, TOTAL - count ) );

    // Report what we sent and add it to the total.
    printf( "Received: %d\n", n );
    count += n;
  }

  // Close the socket connection
  close( sock );

  return 0;
}
