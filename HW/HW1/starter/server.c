#include "common.h"
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
static int gotSignal = 0;
// Print out an error message and exit.
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(1);
}
char board[50];
// Flag for telling the server to stop running because of a sigint.
// This is safer than trying to print in the signal handler.
static int running = 1;
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
  int temp_row = -1;
  int temp_col = -1;
  int undo_time = 0;
  if (argc != 2)
  {
    fail("usage: server <board-file>");
  }

  FILE *file = fopen(argv[1], "r");
  while (fread(board, 100, 1, file) != 0)
  {
  }
  for (int i = 0; i < strlen(board); i++)
  {
    if (board[i] != '.' && board[i] != '*' && board[i] != '\n')
    {
      printf("Invalid input file:");
      fail(argv[1]);
    }
  }
  printf("%s", board);
  // Remove both queues, in case, last time, this program terminated
  // abnormally with some queued messages still queued.
  mq_unlink(SERVER_QUEUE);
  mq_unlink(CLIENT_QUEUE);

  // Prepare structure indicating maximum queue and message sizes.
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = MESSAGE_LIMIT;

  // Make both the server and client message queues.
  mqd_t serverQueue = mq_open(SERVER_QUEUE, O_RDONLY | O_CREAT, 0600, &attr);
  mqd_t clientQueue = mq_open(CLIENT_QUEUE, O_WRONLY | O_CREAT, 0600, &attr);
  if (serverQueue == -1 || clientQueue == -1)
    fail("Can't create the needed message queues");
  char succ[] = "success";
  char err[] = "error";
  // Repeatedly read and process client messages.
  while (running)
  {

    char temp_res[RESULT_SIZE];
    char command[RESULT_SIZE];
    // ./client report
    // ./client TODO:
    // ./server board-3.txt
    // command_to_send=argv[1]+" "+argv[2]+" "+argv[3]

    int com = mq_receive(serverQueue, command, MESSAGE_LIMIT, 0);
    if (com != -1)
    {
      if (strcmp(command, "report") == 0)
      {
        strcpy(temp_res, board);
        mq_send(clientQueue, temp_res, strlen(temp_res) + 1, 0);
        printf("Success");
        continue;
      }
      else if (command[0] == 'm')
      {
        temp_row = command[5] - '0';
        temp_col = command[7] - '0';
        if (temp_row >= 0 && temp_row <= 4 && temp_col >= 0 && temp_row <= 4)
        {
          undo_time = 1;
          Move(temp_row, temp_col);
          strcpy(temp_res, succ);
          mq_send(clientQueue, temp_res, strlen(temp_res) + 1, 0);
          printf("Success");
        }
        else
        {
          strcpy(temp_res, err);
          mq_send(clientQueue, temp_res, strlen(temp_res) + 1, 0);
          printf("Error\n");
        }
        continue;
      }
      else if (strcmp(command, "undo") == 0 && undo_time == 1)
      {
        undo_time = 2;
        printf("Success undo\n");
        Move(temp_row, temp_col);
        strcpy(temp_res, succ);
        mq_send(clientQueue, temp_res, strlen(temp_res) + 1, 0);
        continue;
      }
      else if (strcmp(command, "undo") == 0 && undo_time != 1)
      {
        printf("Can't undo again!\n");
        strcpy(temp_res, err);
        mq_send(clientQueue, temp_res, strlen(temp_res) + 1, 0);
        continue;
      }
      else if (strcmp(command, "stop") == 0)
        break;
      else
      {
        strcpy(temp_res, err);
        mq_send(clientQueue, temp_res, strlen(temp_res) + 1, 0);
        printf("Wrong input!\n");
        break;
      }
    }
  }
  // Close our two message queues (and delete them).
  mq_close(clientQueue);
  mq_close(serverQueue);

  mq_unlink(SERVER_QUEUE);
  mq_unlink(CLIENT_QUEUE);

  return 0;
}
