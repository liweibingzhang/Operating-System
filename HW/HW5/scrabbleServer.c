#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
int ROW = 0;
int COL = 0;
// check if it is the first word on the board
int word_num = 0;
bool check_connect = false;
#define MAX_LINE_LENGTH 100 // the maximun character of a word
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
/** Port number used by my server */
#define PORT_NUMBER "26028"

/** Maximum word length */
#define WORD_LIMIT 26
char **dict; // the dictionary
#define NUM_THREAD 2
// Print out an error message and exit.
static void fail(char const *message)
{
  fprintf(stderr, "%s\n", message);
  exit(EXIT_FAILURE);
}

/** handle a client connection, close it when we're done. */
void readWord()
{
  FILE *fp1 = fopen("words", "r");
  if (fp1 == NULL)
  {
    printf("File cannot open!");
    exit(0);
  }
  char temp_line[MAX_LINE_LENGTH];
  dict = (char **)malloc(102400 * sizeof(char *));
  for (int i = 0; i < 102400; i++) // read the file and save words to lines
  {
    fgets(temp_line, MAX_LINE_LENGTH, fp1);
    temp_line[strcspn(temp_line, "\r\n")] = '\0';
    dict[i] = (char *)malloc((strlen(temp_line) + 1) * sizeof(char));
    strcpy(dict[i], temp_line);
  }
  fclose(fp1);
}
void *handleClient(void *arg)
{
  char board[ROW][COL];
  memset(board, 0, ROW * COL * sizeof(char));
  // Here's a nice trick, wrap a C standard IO FILE around the
  // socket, so we can communicate the same way we would read/write
  // a file.
  char word[WORD_LIMIT];
  FILE *fp = fdopen(*(int *)arg, "a+");
  int temp_row = 0;
  int temp_col = 0;
  // Prompt the user for a command.
  fprintf(fp, "cmd> ");
  // Temporary values for parsing commands.
  char cmd[10];
  int isU = 0;
  while (fscanf(fp, "%10s", cmd) == 1 &&
         strcmp(cmd, "quit") != 0)
  {
    if (strcmp(cmd, "across") == 0)
    {
      word_num++;
      if (word_num == 1)
      {
        memset(word, 0, sizeof(word));
        fscanf(fp, "%d %d %26s", &temp_row, &temp_col, word);
        for (int i = 0; i < strlen(word); i++)
        {
          if (isupper(word[i]))
          {
            isU = 1;
            break;
          }
        }
        if (isU)
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        if (((temp_col + strlen(word)) > COL) || (temp_row >= ROW))
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        pthread_mutex_lock(&lock);
        for (int i = 0; i < strlen(word); i++)
        {
          board[temp_row][temp_col + i] = word[i];
        }
        pthread_mutex_unlock(&lock);
      }
      else
      {
        memset(word, 0, sizeof(word));
        fscanf(fp, "%d %d %26s", &temp_row, &temp_col, word);
        for (int i = 0; i < strlen(word); i++)
        {
          if (isupper(word[i]))
          {
            isU = 1;
            break;
          }
        }
        if (isU)
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        if (((temp_col + strlen(word)) > COL) || (temp_row >= ROW))
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        for (int i = 0; i < strlen(word); i++)
        {
          if (board[temp_row][temp_col + i] == word[i] || board[temp_row][temp_col + i] == 0)
            check_connect = true;
          else
          {
            check_connect = false;
            break;
          }
        }
        if (!check_connect)
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        if (check_connect)
        {
          pthread_mutex_lock(&lock);
          for (int i = 0; i < strlen(word); i++)
          {
            board[temp_row][temp_col + i] = word[i];
          }
          pthread_mutex_unlock(&lock);
          check_connect = false;
        }
      }
    }
    if (strcmp(cmd, "down") == 0)
    {
      word_num++;
      if (word_num == 1)
      {
        memset(word, 0, sizeof(word));
        fscanf(fp, "%d %d %26s", &temp_row, &temp_col, word);
        if (((temp_row + strlen(word)) > ROW || (temp_col >= COL)))
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        pthread_mutex_lock(&lock);
        for (int i = 0; i < strlen(word); i++)
        {
          board[temp_row + i][temp_col] = word[i];
        }
        pthread_mutex_unlock(&lock);
      }
      else
      {
        memset(word, 0, sizeof(word));
        fscanf(fp, "%d %d %26s", &temp_row, &temp_col, word);
        if (((temp_row + strlen(word)) > ROW || (temp_col >= COL)))
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        for (int i = 0; i < strlen(word); i++)
        {
          if (board[temp_row + i][temp_col] == 0 || board[temp_row + i][temp_col] == word[i])
          {
            check_connect = true;
          }
          else
          {
            check_connect = false;
            break;
          }
        }
        if (!check_connect)
        {
          fprintf(fp, "Invalid command\n");
          fprintf(fp, "cmd> ");
          continue;
        }
        if (check_connect)
        {
          pthread_mutex_lock(&lock);
          for (int i = 0; i < strlen(word); i++)
          {
            board[temp_row + i][temp_col] = word[i];
          }
          pthread_mutex_unlock(&lock);
          check_connect = false;
        }
      }
    }
    if (strcmp(cmd, "board") == 0)
    {
      fprintf(fp, "+");
      for (int i = 0; i < COL; i++)
      {
        fprintf(fp, "-");
      }
      fprintf(fp, "+");
      fprintf(fp, "\n");
      for (int i = 0; i < ROW; i++)
      {
        fprintf(fp, "|");
        for (int j = 0; j < COL; j++)
        {
          if (board[i][j] == 0)
          {
            fprintf(fp, " ");
          }
          else
            fprintf(fp, "%c", board[i][j]);
        }
        fprintf(fp, "|");
        fprintf(fp, "\n");
      }
      fprintf(fp, "+");
      for (int i = 0; i < COL; i++)
      {
        fprintf(fp, "-");
      }
      fprintf(fp, "+");
      fprintf(fp, "\n");
    }
    // Prompt the user for the next command.
    fprintf(fp, "cmd> ");
  }
  // Close the connection with this client.
  fclose(fp);
  return NULL;
}

// bool checkWord()
// {
//   int i, j, k = 0;
//   for (int tp_row = 0; tp_row < ROW; tp_row++)
//   {
//     for (int tp_col = 0; tp_col < COL; tp_col++)
//     {
//       // check string from its right
//       for (i = tp_col; i < COL && board[tp_row][i] != 0; i++)
//       {
//         char str[WORD_LIMIT] = {0};
//         // append all characters into a string
//         for (j = tp_col, k = 0; j <= i; j++, k++)
//         {
//           str[k] = board[tp_row][j];
//         }

//         // check if it is in the dict
//         for (j = 0; j < 102400; j++)
//         {
//           if (strcmp(str, dict[j]) == 0)
//           {
//             return true;
//           }
//         }
//         for (i = tp_row; i < ROW && board[i][tp_col] != 0; i++)
//         {
//           char str[WORD_LIMIT] = {0};

//           // append all characters into a string
//           for (j = tp_row, k = 0; j <= i; j++, k++)
//           {
//             str[k] = board[j][tp_col];
//           }

//           // check if it is in the dict
//           for (j = 0; j < 102400; j++)
//           {
//             if (strcmp(str, words[j]) == 0)
//             {
//               return true;
//             }
//           }
//         }
//       }
//       // check the string below it
//       for (i = tp_row, j = tp_col; i < ROW && j < COL && board[i][j] != 0; i++, j++)
//       {
//         char str[WORD_LIMIT] = {0};
//         // append them into a string
//         for (k = 0; i <= ROW && j <= COL && board[i][j] != 0; i++, j++, k++)
//         {
//           str[k] = board[i][j];
//         }
//         // check if it is in the dict
//         for (k = 0; k < 102400; k++)
//         {
//           if (strcmp(str, words[k]) == 0)
//           {
//             return true;
//           }
//         }
//       }
//     }
//   }
//   return false;
// }
int main(int argc, char *argv[])
{
  ROW = atoi(argv[1]);
  COL = atoi(argv[2]);
  pthread_t handle[NUM_THREAD];
  //  Prepare a description of server address criteria.
  struct addrinfo addrCriteria;
  memset(&addrCriteria, 0, sizeof(addrCriteria));
  addrCriteria.ai_family = AF_INET;
  addrCriteria.ai_flags = AI_PASSIVE;
  addrCriteria.ai_socktype = SOCK_STREAM;
  addrCriteria.ai_protocol = IPPROTO_TCP;

  // Lookup a list of matching addresses
  struct addrinfo *servAddr;
  if (getaddrinfo(NULL, PORT_NUMBER, &addrCriteria, &servAddr))
    fail("Can't get address info");

  // Try to just use the first one.
  if (servAddr == NULL)
    fail("Can't get address");

  // Create a TCP socket
  int servSock = socket(servAddr->ai_family, servAddr->ai_socktype,
                        servAddr->ai_protocol);
  if (servSock < 0)
    fail("Can't create socket");

  // Bind to the local address
  if (bind(servSock, servAddr->ai_addr, servAddr->ai_addrlen) != 0)
    fail("Can't bind socket");

  // Tell the socket to listen for incoming connections.
  if (listen(servSock, 5) != 0)
    fail("Can't listen on socket");

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  // Fields for accepting a client connection.
  struct sockaddr_storage clntAddr; // Client address
  socklen_t clntAddrLen = sizeof(clntAddr);
  readWord();
  int i = 0;
  while (true)
  {
    // Accept a client connection.
    int sock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
    pthread_create(&handle[i], NULL, handleClient, &sock);
    pthread_detach(handle[i]);
    i++;
  }
  // Stop accepting client connections (never reached).
  close(servSock);
  return 0;
}