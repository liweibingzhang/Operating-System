// This program uses named semaphores.  These are great for synchronizing
// between different processes, but they generally aren't needed (and you
// shouldn't use them) for synchronizing threads within the same process.
// That's what anonymous semaphores are for.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <semaphore.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

int main() {
  // First, destroy any old copies the tag semaphore, just in case
  // a previous execution of this program crashed and left the
  // semaphore in an unknown state.
  sem_unlink( "/tag_semaphore" );

  // Make a named semaphore.  The tagSrc program uses the same name
  // when it calls sem_open, so it will get access to the same
  // semaphore (provided we're running on the same host).
  sem_t *tagSem = sem_open( "/tag_semaphore", O_CREAT, 0600, 0 );
  if ( tagSem == SEM_FAILED )
    fail( "Can't make tag semaphore" );

  // Wait for the tagSrc process.
  sem_wait( tagSem );

  // Close and destroy the tag semaphore.
  sem_close( tagSem );
  sem_unlink( "/tag_semaphore" );
}
