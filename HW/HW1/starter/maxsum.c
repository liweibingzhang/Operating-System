#include <unistd.h>
#include "common.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>

// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}

// Print out a usage message, then exit.
static void usage() {
  printf( "usage: maxsum <workers>\n" );
  printf( "       maxsum <workers> report\n" );
  exit( 1 );
}

// Input sequence of values.
int *vList;

// Number of values on the list.
int vCount = 0;

// Capacity of the list of values.
int vCap = 0;

// Read the list of values.
void readList() {
  // Set up initial list and capacity.
  vCap = 5;
  vList = (int *) malloc( vCap * sizeof( int ) );

  // Keep reading as many values as we can.
  int v;
  while ( scanf( "%d", &v ) == 1 ) {
    // Grow the list if needed.
    if ( vCount >= vCap ) {
      vCap *= 2;
      vList = (int *) realloc( vList, vCap * sizeof( int ) );
    }

    // Store the latest value in the next array slot.
    vList[ vCount++ ] = v;
  }
}

int main( int argc, char *argv[] ) {
  bool report = false;
  int workers = 4;
  // Parse command-line arguments.
  if ( argc < 2 || argc > 3 )
    usage();

  if ( sscanf( argv[ 1 ], "%d", &workers ) != 1 ||
       workers < 1 )
    usage();

  // If there's a second argument, it better be the word, report
  if ( argc == 3 ) {
    if ( strcmp( argv[ 2 ], "report" ) != 0 )
      usage();
    report = true;
  }

  readList();
  // You get to add the rest.

  // ...
  int pfd[2];
  if ( pipe( pfd ) != 0 )
    fail( "Can't create pipe" );
  int i = 0;
  pid_t pid = getpid();     //pid value as the value of parent process
  for (i = 0; i < workers - 1; i++) // parent process is also a worker，so create a number of child process of (workers-1)。
  {
    pid = fork();
    if (pid == 0)                //child process
    {
      break;
    }
    else if (pid == -1)
    {
      fail( "Can't create child process" );
    }
  }
  if (pid == 0)           //close unnecessray fd
    close(pfd[0]);
  else{
    close(pfd[1]);
  }
  int sum = 0, max = 0;
  for (int j = i; j < vCount; j += workers)
  {
    sum = 0;
    for (int k = j; k < vCount; k++)
    {
      sum += vList[k];
      if (sum > max)
      {
        max = sum;
      }
    }
  }
  if(report){
    printf("I'm process %d. The maximum sum I found is %d.\n", getpid(), max);
  }
  if (pid == 0)
  {
    lockf(pfd[1], F_LOCK, 0);
    write(pfd[1], &max, sizeof(int));
    lockf(pfd[1], F_ULOCK, 0);
    close(pfd[1]);
    exit(0);
  }
  if (pid != 0){
    for (int m = 0; m < workers - 1;m++){
      int temp = 0;
      read(pfd[0], &temp, sizeof(int));
      if (temp > max)
      {
        max = temp;
      }
    }
    printf("Maximum Sum: %d\n", max);
    close(pfd[0]);
  }
    return 0;
}
