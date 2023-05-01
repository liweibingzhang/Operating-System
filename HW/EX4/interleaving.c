#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>    // for write
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit
// Print out an error message and exit.
static void fail( char const *message ) {
  fprintf( stderr, "%s\n", message );
  exit( 1 );
}
void* fun1(void* arg)
{
    write(STDOUT_FILENO, "a", 1);
    write(STDOUT_FILENO, "b", 1);
}
void* fun2(void* arg)
{


    write(STDOUT_FILENO, "c", 1);
    write(STDOUT_FILENO, "d", 1);

}
void* fun3(void* arg)
{

    write(STDOUT_FILENO, "e", 1);
    write(STDOUT_FILENO, "f", 1);
}
// Define the start routines for your three threads.  Each thread will
// use two calls to write() to print its two characters, then it will
// terminate.

// ...

int main( int argc, char *argv[] ) {
  // A bunch of times.
  for ( int i = 0; i < 100000; i++ ) {
    // Make three threads.
    pthread_t thread_1;
    pthread_t thread_2;
    pthread_t thread_3;
    // ...

    // Join with the three threads.
    pthread_create(&thread_1,NULL,fun1,NULL);
    pthread_create(&thread_2,NULL,fun2,NULL);
    pthread_create(&thread_3,NULL,fun3,NULL);
    // ...
    pthread_join(thread_1,NULL);
    pthread_join(thread_2,NULL);
    pthread_join(thread_3,NULL);
    //sem_destroy(&sem[0]);   
    //sem_destroy(&sem[1]);  
    //sem_destroy(&sem[2]);
    // Use the write system call to print out a newline.  The string
    // we're passing to write is null terminated (since that's what
    // double quotesd strings are in C), but we're just using the
    // first byte (the newline).  Write doesn't care about null
    // terminated strings, it just writes out any sequence of bytes
    // you ask it to.
    write( STDOUT_FILENO, "\n", 1 );
  }

  return 0;
}
