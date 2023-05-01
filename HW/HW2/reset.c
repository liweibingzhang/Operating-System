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
char board_read[50];
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("%s", argv[1]);
        printf("Invalid input file: bad-filename\n");
        exit(0);
    }
    FILE *file = fopen(argv[1], "r");
    if(file==NULL){
        printf("Invalid input file: bad-filename\n");
        exit(0);
    }
    while (fread(board_read, 100, 1, file) != 0)
    {
    }
    for (int i = 0; i < strlen(board_read); i++)
    {
        if (board_read[i] != '.' && board_read[i] != '*' && board_read[i] != '\n')
        {
            printf("Invalid input file:");
            exit(0);
        }
    }
    printf("%s", board_read);
    int shm_fd = 0;
    shm_fd = shmget(IPC_PRIVATE, 1000, IPC_CREAT | 0600);
    if (shm_fd < 0)
    {
        printf("shmget error\n");
        exit(0);
    }
    /* memory map the shared memory segment */
    printf("shmid:%d\n", shm_fd);
    GameState *gs = (GameState *)shmat(shm_fd, NULL, 0); //(void *)
    strcpy(gs->board, board_read);
    gs->undo = 0;
    gs->temp_row = -1;
    gs->temp_col = -1;
    shmdt((GameState *)gs);
    return 0;
}