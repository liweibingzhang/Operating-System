// Sample program showing a race condition resulting
// from concurrent access/modification of memory.

#include <stdio.h>
#include <unistd.h>    // for usleep
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// A variable visible to both threads. */
static volatile int val = 0;

/** Function to increment var 100 million times. */
void *thread1( void *arg ) {
  for ( int i = 0; i < 100000000; i++ )
    val++;

  return NULL;
}

/** Same as thread1, but this one decrements. */
void *thread2( void *arg ) {
  for ( int i = 0; i < 100000000; i++ )
    val--;

  return NULL;
}

int main( int argc, char *argv[] ) {
  // ID for each thread.
  pthread_t tid1, tid2;

  if ( pthread_create( &tid1, NULL, thread1, NULL ) != 0 )
    fail( "Can't create a thread1\n" );
  if ( pthread_create( &tid2, NULL, thread2, NULL ) != 0 )
    fail( "Can't create a new thread2\n" );

  // Just wait for the two threads to finish.
  pthread_join( tid1, NULL );
  pthread_join( tid2, NULL );

  // See what we got in the shared variable.
  printf( "val: %d\n", val );

  return 0;
}
