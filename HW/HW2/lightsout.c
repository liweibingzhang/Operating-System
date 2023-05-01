#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>
#include "common.h"
// 再设置一段共享内存，储存undo的情况。
//  Print out an error message and exit.
char board[50];
// Flag for telling the server to stop running because of a sigint.
// This is safer than trying to print in the signal handler.
void Switch(int row, int col)
{
  if (board[row * 6 + col] == '*')
    board[row * 6 + col] = '.';
  else if (board[row * 6 + col] == '.')
    board[row * 6 + col] = '*';
}
void Move(int row, int col)
{
  Switch(row, col);
  Switch(row - 1, col);
  Switch(row + 1, col);
  Switch(row, col + 1);
  Switch(row, col - 1);
}
int main(int argc, char *argv[])
{
  int shm_fd = 0;
  int temp_row = -1;
  int temp_col = -1;
  GameState *gs = (GameState *)shmat(shm_fd, NULL, 0);
  strcpy(board, gs->board);
  if (argc >= 5)
  {
    printf("error\n");
    exit(0);
  }
  if (strcmp(argv[1], "report") == 0)
  {
    printf("%s", board);
    exit(0);
  }
  if (strcmp(argv[1], "move") == 0)
  {
    temp_row = atoi(argv[2]);
    temp_col = atoi(argv[3]);
    printf("%d", temp_row);
    printf("%d", temp_col);
    if (temp_row >= 0 && temp_row <= 4 && temp_col >= 0 && temp_col <= 4)
    {
      Move(temp_row, temp_col);
      strcpy(gs->board, board);
      gs->undo = 1;
      gs->temp_row = temp_row;
      gs->temp_col = temp_col;
      printf("Success");
      exit(0);
    }
    else
    {
      printf("error\n");
      exit(0);
    }
  }
  if (strcmp(argv[1], "undo") == 0 && gs->undo == 1)
  {
    printf("Success");
    printf("row: %d\n", gs->temp_row);
    printf("col: %d\n", gs->temp_col);
    Move(gs->temp_row, gs->temp_col);
    strcpy(gs->board, board);
    gs->undo = '2';
    exit(0);
  }
  else if (strcmp(argv[1], "undo") == 0 && gs->undo != 1)
  {
    printf("error\n");
    exit(0);
  }
  else
  {
    printf("error\n");
    exit(0);
  }
  shmdt((GameState *)gs);
  return 0;
}
