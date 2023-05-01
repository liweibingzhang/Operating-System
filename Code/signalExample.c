#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static int gotSignal = 0;

// Function called when the alarm goes off.
void alarmHandler( int sig ) {
  // Report that we got the alarm and then get main to exit.
  // Really, I don't think it's safe to use printf in a signal handler.
  printf( "Got alarm\n" );
  gotSignal = 1;
}

int main() {
  struct sigaction act;

  // Fill in a structure to redirect the alarm signal.
  act.sa_handler = alarmHandler;
  sigemptyset( &( act.sa_mask ) );
  act.sa_flags = 0;
  sigaction( SIGINT, &act, 0 );

  // Print out our PID, to make it easy for another process to signal us
  printf( "My PID: %d\n", getpid() );

  // You have 10 seconds to type in a message.
  alarm( 10 );

  // Try to read a line from the user (but we only have so much time
  // to read it).
  char buffer[ 1024 ];
  int len = read( STDIN_FILENO, buffer, sizeof( buffer ) );
  if ( len > 0 )
    printf( "You Said: %.*s", len, buffer );
  
  if ( gotSignal )
    printf( "Too Slow\n" );

  return 0;
}
