#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <semaphore.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

int main() {
  // Open the semaphore already made by tagDest.
  sem_t *tagSem = sem_open( "/tag_semaphore", 0 );
  if ( tagSem == SEM_FAILED )
    fail( "Can't open tag semaphore" );

  // Tell the tagDest process it can continue.
  sem_post( tagSem );

  // Close the semaphore and exit.
  sem_close( tagSem );
}
