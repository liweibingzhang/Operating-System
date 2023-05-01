#include <stdio.h>
#include <stdarg.h>    // for varargs
#include <stdlib.h>    // for exit
#include <unistd.h>    // for fork
#include <sys/wait.h>  // for wait
#include <sys/types.h>

/** General purpose fail function.  Print a message and exit. */
static void fail( char const *format, ... );

int main() {
  // Create then join a bunch of threads.
  for ( int i = 0; i < 10000; i++ ) {
    pid_t pid;

    // make the child process
    if ( ( pid = fork() ) < 0 )
      fail( "Not able to fork a child" );

    // If I'm the child, exit right away.
    if ( pid == 0 )
      exit( 0 );
    
    // Wait for the child to terminate.
    wait( 0 );
  }
}

static void fail( char const *format, ... ) {
  va_list param;
  va_start( param, format );

  vprintf( format, param );
  va_end( param );

  exit( -1 );
}
