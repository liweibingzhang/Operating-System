// Sample program without race condition using posix semaphores.

#include <stdio.h>
#include <unistd.h>    // for usleep
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit
#include <semaphore.h> // for posix semaphores

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// A variable used to tell the threads when to stop.
static volatile int running = 1;

// A variable visible to both threads. */
static volatile int val = 0;

/** Semaphore to protect access to critical section. */
sem_t lock;

/** Function to add and subtract by just 1. */
void *thread0( void *arg ) {
  // Try to increment then decrement a shared variable a bunch of times.
  while ( running ) {
    // Entry code
    sem_wait( &lock );
    // Critical section
    val++;
    val--;
    // Exit code.
    sem_post( &lock );
    // Remainder section (empty, but atypical)

  }

  return NULL;
}

/** Another thread doing exactly the same thing. */
void *thread1( void *arg ) {
  // Try to increment then decrement a shared variable a bunch of times.
  while ( running ) {
    // Entry code
    sem_wait( &lock );


    // Critical section
    val++;
    val--;

    // Exit code.
    sem_post( &lock );

    // Remainder section (empty, but atypical)

  }

  return NULL;
}

int main( int argc, char *argv[] ) {
  // ID for each thread.
  pthread_t tid0, tid1;

  // Initialize our semaphore to one.
  sem_init( &lock, 0, 1 );

  if ( pthread_create( &tid0, NULL, thread0, NULL ) != 0 ||
       pthread_create( &tid1, NULL, thread1, NULL ) != 0 )
    fail( "Can't create a new thread\n" );

  // Let the threads run for a couple of seconds.
  sleep( 2 );

  // Now, ask them to stop.
  running = 0;

  // Just wait for the two threads to finish.
  pthread_join( tid0, NULL );
  pthread_join( tid1, NULL );

  // See what we got in the shared variable.
  printf( "val: %d\n", val );

  // Free the semaphore
  sem_destroy( &lock );

  return 0;
}
