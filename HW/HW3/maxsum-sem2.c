#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <semaphore.h>
#include <sys/syscall.h>

// Print out an error message and exit.
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}

// Print out a usage message, then exit.
static void usage()
{
  printf("usage: maxsum-sem <workers>\n");
  printf("       maxsum-sem <workers> report\n");
  exit(1);
}
typedef struct
{
  int number;   // 工人的编号
  int workers;  // 工人的数量
  int maxvalue; // 线程所求的最大值
} arglist;
sem_t work_done;
int next_idx = 0;
// True if we're supposed to report what we find.
bool report = false;

// Maximum sum we've found.
int max_sum = INT_MIN;

// Fixed-sized array for holding the sequence.
#define MAX_VALUES 500000
int vList[MAX_VALUES];

// Current number of values on the list.
int vCount = 0;

// Read the list of values.
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
    sem_post(&work_done);
  }
  for(){
    sem_post(&work_done);
  }
}
int getwork()
{
  int last_idx = -1;
  int this_idx = -1;
  sem_wait(&work_done);
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
      sem_post(&work_done); // signal that more work is available
    }   
    return this_idx;
}
/** Start routine for each worker. */
void *workerRoutine(void *arg)
{
  int getNum = getwork();
  arglist *args = arg;
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
  if (report)
  {
    // printf("I'm thread %ld. The maximum sum I found is %d.\n", pthread_self(), max);
  }
}
void initarg(arglist *arg, int i, int worker)
{
  arg->number = i;
  arg->workers = worker;
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

  // If there's a second argument, it better be "report"
  if (argc == 3)
  {
    if (strcmp(argv[2], "report") != 0)
      usage();
    report = true;
  }
  sem_init(&work_done, 0, 1);
  // Then, start getting work for them to do.
  readList();
  // Make each of the workers.
  pthread_t worker[workers];
  arglist pargs[workers];
  for (int i = 0; i < workers; i++)
  {
    initarg(&pargs[i], i, workers);
    bool flag = pthread_create(&worker[i], NULL, workerRoutine, &pargs[i]);
    if (flag)
    {
      fail("Creating thread fails");
    }
  }
  for (int i = 0; i < workers; i++)
  {
    pthread_join(worker[i], NULL);
  }
  int max = -9999;

  // Wait until all the workers finish.
  for (int i = 0; i < workers; i++)
  {
    if (max < pargs[i].maxvalue)
      max = pargs[i].maxvalue;
  }
  // Report the max product and release the semaphores.
  printf("Maximum Sum: %d\n", max_sum);
  sem_destroy(&work_done);
  return EXIT_SUCCESS;
}
