#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

/** A little record used to keep up with each of our threads. */
typedef struct
{
  // Thread handle for this chef.
  pthread_t thread;

  // Name for this chef.
  char name[25];

  // Pointer to the start routine for this thread.
  void *(*start)(void *);

  // Number of dishes prepared by this chef.
  int dishCount;
} ChefRec;

/** To tell all the chefs when they can quit running. */
static bool running = true;

/** Print out an error message and exit. */
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}
/** Called by a chef after they have locked all the required appliances
    and are ready to cook for about the given number of milliseconds. */
static void cook(int duration, ChefRec *chef)
{
  printf("%s is cooking\n", chef->name);
  // Wait for between duration / 2 and duration ms
  usleep(500 * ((long)rand() * duration / RAND_MAX + duration));
  chef->dishCount++;
}

/** Called by a chef between dishes, to let them rest about the given
    number of milliseconds before cooking another dish. */
static void rest(int duration, ChefRec *chef)
{
  printf("%s is resting\n", chef->name);
  // Wait for between duration / 2 and duration ms
  usleep(500 * ((long)rand() * duration / RAND_MAX + duration));
}

// A mutex representing the lock on each appliance.
// Acquiring the needed mutexes before cooking prevents two
// chefs from trying to use the same appliance at the same time.
pthread_mutex_t appliance_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t appliance_available = PTHREAD_COND_INITIALIZER;
bool griddle = true;
bool mixer = true;
bool oven = true;
bool blender = true;
bool grill = true;
bool fryer = true;
bool microwave = true;
bool coffeeMaker = true;

/** Mandy is a chef needing 105 milliseconds to prepare a dish. */
void *Mandy(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!microwave || !coffeeMaker)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    microwave = false;
    coffeeMaker = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(105, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    microwave = true;
    coffeeMaker = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Edmund is a chef needing 30 milliseconds to prepare a dish. */
void *Edmund(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!blender || !oven || !mixer)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    blender = false;
    oven = false;
    mixer = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(30, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    blender = true;
    oven = true;
    mixer = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Napoleon is a chef needing 60 milliseconds to prepare a dish. */
void *Napoleon(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!blender || !grill)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    blender = false;
    grill = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(60, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    blender = true;
    grill = true;
    pthread_cond_broadcast(&appliance_available);

    rest(25, rec);
  }

  return NULL;
}

/** Prudence is a chef needing 15 milliseconds to prepare a dish. */
void *Prudence(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    pthread_mutex_lock(&appliance_mutex);
    while (!griddle || !microwave || !coffeeMaker)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    griddle = false;
    microwave = false;
    coffeeMaker = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(15, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    griddle = true;
    microwave = true;
    coffeeMaker = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Kyle is a chef needing 45 milliseconds to prepare a dish. */
void *Kyle(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!oven || !fryer)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    oven = false;
    fryer = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(45, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    oven = true;
    fryer = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Claire is a chef needing 15 milliseconds to prepare a dish. */
void *Claire(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!griddle || !grill)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    griddle = false;
    grill = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(15, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    griddle = true;
    grill = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Lucia is a chef needing 15 milliseconds to prepare a dish. */
void *Lucia(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!griddle || !mixer)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    griddle = false;
    mixer = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(15, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    griddle = true;
    mixer = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Marcos is a chef needing 60 milliseconds to prepare a dish. */
void *Marcos(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!blender || !fryer || !microwave)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    blender = false;
    fryer = false;
    microwave = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(60, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    blender = true;
    fryer = true;
    microwave = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Roslyn is a chef needing 75 milliseconds to prepare a dish. */
void *Roslyn(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    // Get the appliances this chef uses.
    pthread_mutex_lock(&appliance_mutex);
    while (!grill || !fryer)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    grill = false;
    fryer = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(75, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    grill = true;
    fryer = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

/** Stephenie is a chef needing 30 milliseconds to prepare a dish. */
void *Stephenie(void *arg)
{
  // Argument struct, converted to its actual type.
  ChefRec *rec = (ChefRec *)arg;

  while (running)
  {
    pthread_mutex_lock(&appliance_mutex);
    while (!coffeeMaker || !oven || !mixer)
    {
      pthread_cond_wait(&appliance_available, &appliance_mutex);
    }
    coffeeMaker = false;
    oven = false;
    mixer = false;
    pthread_mutex_unlock(&appliance_mutex);
    cook(30, rec);
    pthread_mutex_lock(&appliance_mutex);
    pthread_mutex_unlock(&appliance_mutex);
    coffeeMaker = true;
    oven = true;
    mixer = true;
    pthread_cond_broadcast(&appliance_available);
    rest(25, rec);
  }

  return NULL;
}

int main(void)
{
  // Seed the random number generator, so we get variation in behavior.
  srand(time(NULL));

  // Make a record for each chef.
  ChefRec chefList[] = {
      {.name = "Mandy", .start = Mandy},
      {.name = "Edmund", .start = Edmund},
      {.name = "Napoleon", .start = Napoleon},
      {.name = "Prudence", .start = Prudence},
      {.name = "Kyle", .start = Kyle},
      {.name = "Claire", .start = Claire},
      {.name = "Lucia", .start = Lucia},
      {.name = "Marcos", .start = Marcos},
      {.name = "Roslyn", .start = Roslyn},
      {.name = "Stephenie", .start = Stephenie},
  };

  // Make a thread for each chef.
  for (int i = 0; i < sizeof(chefList) / sizeof(chefList[0]); i++)
  {
    // Give each chef a pointer to its ChefRec struct.
    if (pthread_create(&chefList[i].thread,
                       NULL,
                       chefList[i].start,
                       chefList + i) != 0)
      fail("Can't create thread");
  }

  // Let the chefs cook for a while, then ask them to stop.
  sleep(10);
  running = false;

  // Wait for all our chefs to finish, and collect up how much
  // cooking was done.
  int total = 0;
  for (int i = 0; i < sizeof(chefList) / sizeof(chefList[0]); i++)
  {
    pthread_join(chefList[i].thread, NULL);
    printf("%s cooked %d dishes\n",
           chefList[i].name,
           chefList[i].dishCount);
    total += chefList[i].dishCount;
  }
  printf("Total dishes cooked: %d\n", total);
}
