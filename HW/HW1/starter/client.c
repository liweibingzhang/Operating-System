#include "common.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
static void fail(char const *message)
{
    fprintf(stderr, "%s\n", message);
    exit(1);
}
char temp_rec[RESULT_SIZE];
char command[RESULT_SIZE];
int main(int argc, char *argv[]){

    mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
    mqd_t clientQueue = mq_open(CLIENT_QUEUE, O_RDONLY);
    if (serverQueue == -1 || clientQueue == -1)
        fail("Can't open message queue");
    if (argc == 2)
    {
        strcpy(command, argv[1]);
    }
    if (argc == 4)
    {
        strcpy(command, argv[1]);
        strcat(command, " ");
        strcat(command, argv[2]);
        strcat(command, " ");
        strcat(command, argv[3]);
    }
    int com = mq_send(serverQueue, command, strlen(command)+1, 0);
    int len = mq_receive(clientQueue, temp_rec, MESSAGE_LIMIT, NULL);
    // "Success"
    // "Error"
    // ".....\n
    // ..*..\n
    // .....\n"
    if(com < 0){
        printf("send:%d", com);
    }
    if(len<0){
        printf("received:%d", len);
    }
    if (len > 0)
    {
        printf("%s",temp_rec);
    }
    return 0;
}