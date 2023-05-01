#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/syscall.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( EXIT_FAILURE );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: maxsum-sem <workers>\n" );
  printf( "       maxsum-sem <workers> report\n" );
  exit( 1 );
}

// True if we're supposed to report what we find.
bool report = false;

// Maximum sum we've found.
int max_sum = INT_MIN;

// Fixed-sized array for holding the sequence.
#define MAX_VALUES 500000
int vList[ MAX_VALUES ];

// Current number of values on the list.
int vCount = 0;

// Read the list of values.
void readList() {
  // Keep reading as many values as we can.
  int v;
  while ( scanf( "%d", &v ) == 1 ) {
    // Make sure we have enough room, then store the latest input.
    if ( vCount > MAX_VALUES )
      fail( "Too many input values" );

    // Store the latest value.
    vList[ vCount++ ] = v;
  }
}

/** Start routine for each worker. */
void *workerRoutine( void *arg ) {
  // ...

  return NULL;
}

int main( int argc, char *argv[] ) {
  int workers = 4;
  
  // Parse command-line arguments.
  if ( argc < 2 || argc > 3 )
    usage();
  
  if ( sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    usage();

  // If there's a second argument, it better be "report"
  if ( argc == 3 ) {
    if ( strcmp( argv[ 2 ], "report" ) != 0 )
      usage();
    report = true;
  }

  // Make each of the workers.
  pthread_t worker[ workers ];
  for ( int i = 0; i < workers; i++ )
    // ...

  // Then, start getting work for them to do.
  readList();

  // Wait until all the workers finish.
  for ( int i = 0; i < workers; i++ )
    // ...

  // Report the max product and release the semaphores.
  printf( "Maximum Sum: %d\n", max_sum );
  
  return EXIT_SUCCESS;
}
