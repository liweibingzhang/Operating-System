#include <stdio.h>
#include <unistd.h>    // for usleep
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit

// global variable, visible to both threads.
volatile int sharedVariable = 0;

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

/** A function for our new thread to run. */
void *threadFunc( void *arg ) {
  // periodically increment the global variable.
  for ( int i = 0; i < 15; i++ ) {
    usleep( 500000 );
    sharedVariable += 1;
  }

  return NULL;
}

int main( int argc, char *argv[] ) {
  // Structure used to keep up with our new thread.
  pthread_t thread;

  // Make one new thread and make sure it gets made.
  if ( pthread_create( &thread, NULL, threadFunc, NULL ) != 0 ) 
    fail( "Can't create a child thread" );

  // Print a message over and over, so the user can see two threads running.
  for ( int i = 0; i < 20; i++ ) {
    printf( "Original Thread: %d\n", sharedVariable );
    usleep( 250000 );
  }

  // Wait for our child thread to exit (it will probably run a little
  // bit longer than me)
  pthread_join( thread, NULL );

  printf( "The new thread is done\n" );

  return 0;
}
