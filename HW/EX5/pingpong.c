#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// Number of iterations for each thread
#define ITERATIONS 500

// Who gets to go next.
int nextTurn = 0;

// Print out an error message and exit.
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

// Start routines for the two threads.
void *thread_fun1(void *arg)
{
  for (int i = 0; i < ITERATIONS; i++)
  {
    while (nextTurn != 0)
      ;
    nextTurn = 1;
  }
}
// ...
void *thread_fun2(void *arg)
{
  for (int i = 0; i < ITERATIONS; i++)
  {
    while (nextTurn != 1)
      ;
    nextTurn = 0;
  }
}
int main(int argc, char *argv[])
{

  // Create each of the two threads.

  // ...

  // Wait for them both to finish.

  // ...
  pthread_t thread_id1;
  pthread_t thread_id2;

  pthread_create(&thread_id1, NULL, thread_fun1, NULL);
  pthread_create(&thread_id2, NULL, thread_fun2, NULL);

  // 让线程运行直到结束
  pthread_join(thread_id1, NULL);
  pthread_join(thread_id2, NULL);
  return 0;
}
