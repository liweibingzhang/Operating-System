#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

// Number of iterations for each thread
#define ITERATIONS 500

// Declare two anonymous semaphores, one to let each of the threads
// go next.
sem_t m1;
sem_t m2;
// Print out an error message and exit.
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

// Start routines for the two threads.
void *thread1(void *arg)
{
  for (int i = 0; i < ITERATIONS; i++)
  {
    sem_wait(&m1);
    sem_post(&m2);
  }
}

// ...
void *thread2(void *arg)
{
  for (int i = 0; i < ITERATIONS; i++)
  {
    sem_wait(&m2);
    sem_post(&m1);
  }
}
int main(int argc, char *argv[])
{

  // Create two semephaores, one to let ping go next and one to let
  // pong go next.
  sem_init(&m1, 0, 1);
  sem_init(&m2, 0, 2);
  // Create each of the two threads.
  pthread_t t1, t2;
  pthread_create(&t1, NULL, thread1, NULL);
  //sem_post(&m1);
  pthread_create(&t2, NULL, thread2, NULL);
  //sem_post(&m2);
  // Wait for them both to finish.
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  // Destroy the two semaphores.
  sem_destroy(&m1);
  sem_destroy(&m2);
  // ...

  return 0;
}
