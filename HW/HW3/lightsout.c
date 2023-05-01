#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include "common.h"
char board[50];
sem_t qjin3_lightsout_lock;
void Switch(int row, int col)
{
  if (board[row * 6 + col] == '*')
    board[row * 6 + col] = '.';
  else if (board[row * 6 + col] == '.')
    board[row * 6 + col] = '*';
}
bool move(GameState *gs, int r, int c)
{
#ifndef UNSAFE
  sem_wait(&qjin3_lightsout_lock);
#endif
  if (r < 0 || r > 4 || c < 0 || c > 4)
  {
#ifndef UNSAFE
    sem_post(&qjin3_lightsout_lock);
#endif
    return false;
  }
  else
  {
    Switch(r, c);
    Switch(r - 1, c);
    Switch(r + 1, c);
    Switch(r, c + 1);
    Switch(r, c - 1);
    strcpy(gs->board, board);
    gs->undo = 1;
    gs->temp_row = r;
    gs->temp_col = c;
#ifndef UNSAFE
    sem_post(&qjin3_lightsout_lock);
#endif
    return true;
  }
}
bool undo(GameState *gs)
{
#ifndef UNSAFE
  sem_wait(&qjin3_lightsout_lock);
#endif
  if (gs->undo == 1)
  {
    move(gs, gs->temp_row, gs->temp_col);
    strcpy(gs->board, board);
    gs->undo = 2;
#ifndef UNSAFE
    sem_post(&qjin3_lightsout_lock);
#endif
    return true;
  }
  else
  {
#ifndef UNSAFE
    sem_post(&qjin3_lightsout_lock);
#endif;
    return false;
  }
}
bool test(GameState *gs, int n, int r, int c)
{
#ifndef UNSAFE
  sem_wait(&qjin3_lightsout_lock);
#endif
  // Make sure the row / colunn is valid.
  if (r < 0 || r > 4 || c < 0 || c > 4)
  {
    return false;
  }
  // Make the same move a bunch of times.
  for (int i = 0; i < n; i++)
    move(gs, r, c);
#ifndef UNSAFE
  sem_post(&qjin3_lightsout_lock);
#endif
  return true;
}
void report(GameState *gs)
{
#ifndef UNSAFE
  sem_wait(&qjin3_lightsout_lock);
#endif
  printf("%s", gs->board);
#ifndef UNSAFE
  sem_post(&qjin3_lightsout_lock);
#endif
}
int main(int argc, char *argv[])
{
#ifndef UNSAFE
  sem_init(&qjin3_lightsout_lock, 0, 1);
#endif
  int shm_fd = 0;
  int temp_row = -1;
  int temp_col = -1;
  GameState *gs = (GameState *)shmat(shm_fd, NULL, 0);
  strcpy(board, gs->board);
  gs->undo = 0;
  if (strcmp(argv[1], "report") == 0)
  {
    report(gs);
    exit(0);
  }
  if (strcmp(argv[1], "move") == 0)
  {
    temp_row = atoi(argv[2]);
    temp_col = atoi(argv[3]);
    move(gs, temp_row, temp_col);
    printf("Success");
    exit(0);
  }
  if (strcmp(argv[1], "test") == 0)
  {
    int times = atoi(argv[2]);
    temp_row = atoi(argv[3]);
    temp_col = atoi(argv[4]);
    test(gs, times, temp_row, temp_col);
  }
  if (strcmp(argv[1], "undo") == 0 && gs->undo == 1)
  {
    printf("Success");
    undo(gs);
    exit(0);
  }
  else
  {
    printf("error\n");
    exit(0);
  }
  shmdt((GameState *)gs);
#ifndef UNSAFE
  sem_destroy(&qjin3_lightsout_lock);
#endif
  return 0;
}
