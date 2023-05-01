#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#define Size 1024
static void fail( char *msg ) {
  // This program does redirection of standard output.  Good thing
  // we still have standard error around, so we can print error
  // messages even 
  fprintf( stderr, "Error: %s\n", msg );
  exit( EXIT_FAILURE );
}

int main( int argc, char *argv[] ) {
  // Make a child process to run cat.
  char buffer[Size];
  pid_t pid = fork();
  if ( pid == -1 )
    fail( "Can't create child process" );

  if ( pid == 0 ) {
    // I'm the child.  Before replacing myself with the cat program,
    // change its environment so it reads from "input.txt" instead of
    // standard input.
    int fd1 = open("input.txt", O_RDONLY);
    // ...
    int fd1New;
    fd1New = dup2(fd1, fileno(stdin));
    close(fd1);
    
    // ... and writes to "output.txt" instead of standard output.
    int fd2 = open("output.txt", O_CREAT | O_WRONLY, 0600);
    int fd2New = dup2(fd2, fileno(stdout));
    close(fd2);
    // ...
    // Now, run cat.  Even though it thinks it's reading from standard
    // input and writing to standard output, it will really be copying these
    // files.
    if(execl("/usr/bin/cat", "cat", NULL)==-1){
      fail("execl");
    }
    // ...
  }
  // Wait for the cat program to finish.
  wait( NULL );
  
  return EXIT_SUCCESS;
}
