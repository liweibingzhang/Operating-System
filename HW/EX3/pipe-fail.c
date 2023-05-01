#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void fail( char *msg ) {
  fprintf( stderr, "Error: %s\n", msg );
  exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] ) {
  // Make a pipe for our two children to talk over.
  // ...
  int pfd[2];
  if ( pipe( pfd ) != 0 )
    fail( "Can't create pipe" );
  // Make a child process to run ps -ef.
  pid_t pid = fork();
  if ( pid == -1 )
    fail( "Can't create child process" );

  if ( pid == 0 ) {
    // I'm the child.  Before replacing myself with the ps program,
    // change my environment.  First, close the reading end of the
    // pipe, since I won't need it.
    close(pfd[0]);
    // ...

    // Then, replace my standard output with the writing end of the pipe.
    char buffer[ 1024 ];
    
    // ...

    // I just made a copy of the writing end of the pipe, close the old copy
    // of the writing end

    // ...

    // Now, execl ps -ef

    // ...
    if(execl("/usr/bin/ps", "ps","-ef", NULL)==-1){
        fail( "Can't run ps -ef" );
    }
  }

  // Make another child process to run grep bash
  pid = fork();
  if ( pid == -1 )
    fail( "Can't create second child process" );

  if ( pid == 0 ) {
    // I'm the child.  Before replacing myself with the ps program,
    // change my environment.  First, close the writing end of the
    // pipe, since I won't need it.
    close(pfd[0]);
    // ...

    // Then, replace my standard input with the reading end of the pipe.

    // ...

    // I just made a copy of the reading end of the pipe, close the old copy
    // of the reading end.

    // ...


    // Now, execl grep bash

    // ...
    if(execl("/usr/bin/grep", "grep",,"bash" NULL)==-1){
        fail( "Can't run grep bash" );
    }
  }

  // I'm the parent process.  I don't need either end of the pipe, it's just
  // for childrent to communicate.
  close(pfd[1]);
  // ...

  // Wait for both of the children to terminate.
  wait( NULL );
  wait( NULL );

  return EXIT_SUCCESS;
}
