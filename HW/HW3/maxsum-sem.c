#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
typedef struct
{
  int number;   // 工人的编号
  int workers;  // 工人的数量
  int maxvalue; // 线程所求的最大值
} arglist;
bool report = false;
#define MAX_VALUES 500000
// Print out an error message and exit.
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}

// Print out a usage message, then exit.
static void usage()
{
  printf("usage: maxsum <workers>\n");
  printf("       maxsum <workers> report\n");
  exit(1);
}
sem_t lock;
// Input sequence of values.
int *vList;

// Number of values on the list.
int vCount = 0;

// Capacity of the list of values.
int vCap = 0;

// Read the list of values.
int next_idx = 0;
void readList()
{
  // Keep reading as many values as we can.
  int v;
  while (scanf("%d", &v) == 1)
  {
    // Make sure we have enough room, then store the latest input.
    if (vCount > MAX_VALUES)
      fail("Too many input values");
    // Store the latest value.
    vList[vCount++] = v;
    sem_post(&lock);
  }
  for (int i = 0; i < vCount;i++)
  {
    sem_post(&lock);
  }
}
int getwork()
{
  int last_idx = -1;
  int this_idx = -1;
  sem_wait(&lock);
  if (next_idx >= vCount)
  {
    return -1;
  }
  for (int i = last_idx + 1; i < vCount; i++)
  {
    // Check if this value has not yet been assigned to a worker
    if (i == next_idx)
    {
      this_idx = i;
      next_idx++;
      break;
    }
  }
  last_idx = this_idx; // update last assigned value for this worker    
    if (next_idx < vCount) {
      sem_post(&lock); // signal that more work is available
    }   
    return this_idx;
}
void *workerRoutine(void *ptr)
{
  arglist *args = ptr;
  int i = args->number;
  int max = -9999;
  for (; i < vCount; i += args->workers)
  {
    int sum = 0;
    for (int j = i; j < vCount; j++)
    {
      sum += vList[j];
      if (sum > max)
      {
        max = sum;
      }
    }
  }
  args->maxvalue = max;
}
void initarg(arglist *arg, int i, int worker)
{
  arg->number = i;
  arg->workers = worker;
}
// Read the list of values.
void readlist()
{
  // Set up initial list and capacity.
  vCap = 5;
  vList = (int *)malloc(vCap * sizeof(int));

  // Keep reading as many values as we can.
  int v;
  while (scanf("%d", &v) == 1)
  {
    sem_wait(&lock);
    // Grow the list if needed.
    if (vCount >= vCap)
    {
      vCap *= 2;
      vList = (int *)realloc(vList, vCap * sizeof(int));
    }
    sem_post(&lock);
    // Store the latest value in the next array slot.
    vList[vCount++] = v;
  }
}
int main(int argc, char *argv[])
{
  int workers = 4;
  // Parse command-line arguments.
  if (argc < 2 || argc > 3)
    usage();

  if (sscanf(argv[1], "%d", &workers) != 1 ||
      workers < 1)
    usage();

  // If there's a second argument, it better be the word, report
  if (argc == 3)
  {
    if (strcmp(argv[2], "report") != 0)
      usage();
    report = true;
  }
  sem_init(&lock, 0, 1);
  readlist();
  pthread_t tid[workers];
  arglist pargs[workers];

  for (int i = 0; i < workers; i++)
  {
    initarg(&pargs[i], i, workers);
    bool flag = pthread_create(&tid[i], NULL, workerRoutine, &pargs[i]);
    if (flag)
    {
      fail("Creating thread fails");
    }
  }
  for (int i = 0; i < workers; i++)
  {
    pthread_join(tid[i], NULL);
  }
  int max = -9999;
  for (int i = 0; i < workers; i++)
  {
    if (max < pargs[i].maxvalue)
      max = pargs[i].maxvalue;
  }
  printf("Maximum Sum: %d\n", max);
  sem_destroy(&lock);
  return 0;
}
