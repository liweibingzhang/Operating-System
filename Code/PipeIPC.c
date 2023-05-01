// Demonstration of sending a text message from child process to parent via a
// (anonymous) pipe.

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

int main() {
  // Make a pipe for talking with the child.
  int pfd[ 2 ];
  if ( pipe( pfd ) != 0 )
    fail( "Can't create pipe" );

  // make the child process
  pid_t id = fork();
  if ( id  < 0 )
    fail( "Not able to fork a child" );

  // Decide if I'm the child or parent, based on the return value from fork().
  if ( id == 0 ) {
    // I'm the child, I don't need the reading end of the pipe.
    close( pfd[ 0 ] );
    
    // Read a string message from the user.
    char buffer[ 1024 ];
    printf( "Enter a String: " );
    fgets( buffer, sizeof( buffer ), stdin );

    // Write the contents of the string into the pipe.
    int slen = strlen( buffer );
    int len = write( pfd[ 1 ], buffer, slen );
    if ( len < 0 )
      fail( "Error writing to pipe" );

    // Child is done.  Exiting will close pfd[ 1 ].
    exit( 0 );
  } else {
    // I'm the parent, I don't need the writing end of the pipe.
    close( pfd[ 1 ] );

    // Read bytes from the pipe.  This would not get the whole message if
    // it was longer than 1023 bytes.
    char buffer[ 1024 ];
    int len = read( pfd[ 0 ], buffer, sizeof( buffer ) - 1 );
    if ( len < 0 )
      fail( "Error reading from pipe" );

    // Put a null terminator on the array of bytes and print it like a string.
    buffer[ len ] = '\0';
    printf( "Message: %s", buffer );

    // Wait for our child to exit.
    wait( 0 );
  }

  return EXIT_SUCCESS;
}
