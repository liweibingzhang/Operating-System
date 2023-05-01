#include <stdio.h>
#include <pthread.h>   // for pthreads
#include <stdarg.h>    // for varargs
#include <stdlib.h>    // for exit

/** General purpose fail function.  Print a message and exit. */
static void fail( char const *format, ... );

/** Function run by our new threads. */
void *threadFunc( void *arg ) {
  // Just exit right away.

  return NULL;
}

int main( int argc, char *argv[] ) {
  // Structure used to keep up with each new thread.
  pthread_t thread;

  // Create then join a bunch of threads.
  for ( int i = 0; i < 10000; i++ ) {
    if ( pthread_create( &thread, NULL, threadFunc, NULL ) != 0 ) 
      fail( "Can't create a child thread\n" );

    // Wait for our child thread to exit.
    pthread_join( thread, NULL );
  }

  return 0;
}

static void fail( char const *format, ... ) {
  va_list param;
  va_start( param, format );

  vprintf( format, param );
  va_end( param );

  exit( -1 );
}
