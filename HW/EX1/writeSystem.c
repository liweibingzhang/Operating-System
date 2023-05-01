#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#define ITERATIONS 50000

int main( int argc, char *argv[] ) {
  // Buffer we're going to write to the file, one character at a time.
  char buffer[] = "abcdefghijklmnopqrstuvwxyz\n";
  int bufferSize = strlen( buffer );
  // Create an output file for writing.
  int fd = open("output.txt", O_CREAT | O_WRONLY, 0600);
  if (fd < 0)
  {
    fprintf( stderr, "Can't open output file.\n" );
    exit( EXIT_FAILURE );
  }
  // Make a lot of writes to the file.
  for ( int i = 0; i < ITERATIONS; i++ ) {
    // Write out the buffer one character at a time.  That's not a
    // good idea, but it will help to show what system call overhead
    // is like.
    for ( int j = 0; j < bufferSize; j++ ){
      write(fd, buffer+j, 1);
    }
    // Rewind back to the start of the file, so it doesn't get too long.
    lseek(fd,0, SEEK_SET);
  }

  // Close the file and exit.
  close(fd);
  return 0;
}
