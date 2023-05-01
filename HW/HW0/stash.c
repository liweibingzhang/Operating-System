/* 头文件 */
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>             

/* 宏定义 */
#define IN 1
#define OUT 0
#define MAX_CMD 100
#define BUFFSIZE 1024
#define MAX_CMD_LEN 100


/* 全局变量 */
int argc;                                       // 有效参数个数
char* argv[MAX_CMD];                            // 参数数组
char command[MAX_CMD][MAX_CMD_LEN];             // 参数数组
char buf[BUFFSIZE];                             // 接受输入的参数数组
char backupBuf[BUFFSIZE];                       // 参数数组的备份
char curPath[BUFFSIZE];                         // 当前shell工作路径
int i, j;                                       // 循环变量
int commandNum;                                 // 已经输入指令数目


int get_input(char buf[]);                      // 输入指令并存入buf数组
void parse(char* buf);                          // 解析字符串
void runCommand(int argc, char* argv[]);
int runCd(int argc);                       // 执行cd指令

/* 函数定义 */
/* get_input接受输入的字符并存入buf数组中 */
int get_input(char buf[]) {
    // buf数组初始化
    memset(buf, 0x00, BUFFSIZE);
    memset(backupBuf, 0x00, BUFFSIZE);        
    fgets(buf, BUFFSIZE, stdin);
    // 去除fgets带来的末尾\n字符
    buf[strlen(buf) - 1] = '\0';
    return strlen(buf);
}

void parse(char* buf) {
    // 初始化argv数组和argc
    for (i = 0; i < MAX_CMD; i++) {
        argv[i] = NULL;
        for (j = 0; j < MAX_CMD_LEN; j++)
            command[i][j] = '\0';
    }
    argc = 0;
    // 下列操作改变了buf数组, 为buf数组做个备份
    strcpy(backupBuf, buf);
    /** 构建command数组
     *  即若输入为"ls -a"
     *  strcmp(command[0], "ls") == 0 成立且
     *  strcmp(command[1], "-a") == 0 成立
     */  
    int len = strlen(buf);
    for (i = 0, j = 0; i < len; ++i) {
        if (buf[i] != ' ') {
            command[argc][j++] = buf[i];
        } else {
            if (j != 0) {
                command[argc][j] = '\0';
                ++argc;
                j = 0;
            }
        }
    }
    if (j != 0) {
        command[argc][j] = '\0';
    }

    /** 构建argv数组
     *  即若输入buf为"ls -a"
     *  strcmp(argv[0], "ls") == 0 成立且
     *  strcmp(argv[1], "-a") == 0 成立*/
    argc = 0;
    int flg = OUT;
    for (i = 0; buf[i] != '\0'; i++) {
        if (flg == OUT && !isspace(buf[i])) {
            flg = IN;
            argv[argc++] = buf + i;
        } else if (flg == IN && isspace(buf[i])) {
            flg = OUT;
            buf[i] = '\0';
        }
    }
    argv[argc] = NULL;
}
void runCommand(int argc, char* argv[]) {
    pid_t pid;
    /* 识别shell内置命令 */
    if (strcmp(command[0], "cd") == 0) {
        int res = runCd(argc);
        if (!res) 
            printf("Invalid command\n");
    }
    else if (strcmp(command[0], "exit") == 0) {
        exit(0);
    } else {
        switch(pid = fork()) {
            // fork子进程失败
            case -1:
                printf("Fail to create child process\n");
                return;
            // 处理子进程
            case 0:
                {   /* 函数说明：execvp()会从PATH 环境变量所指的目录中查找符合参数file 的文件名, 找到后便执行该文件, 
                     * 然后将第二个参数argv 传给该欲执行的文件。
                     * 返回值：如果执行成功则函数不会返回, 执行失败则直接返回-1, 失败原因存于errno 中. 
                     * */
                    execvp(argv[0], argv);
                    // 代码健壮性: 如果子进程未被成功执行, 则报错
                    printf("Can’t run command command-that-does-not-exist\n");
                    // exit函数终止当前进程, 括号内参数为1时, 会像操作系统报告该进程因异常而终止
                    exit(1);
                }
            default: {
                    int status;
                    waitpid(pid, &status, 0);      // 等待子进程返回
                    int err = WEXITSTATUS(status); // 读取子进程的返回码
                    if (err) { 
                        printf("Error: %s\n", strerror(err));
                    }                    
            }
        }
    }
}

int runCd(int argc) {
    // result为1代表执行成功, 为0代表执行失败
    int result = 1;
    if (argc != 2) {
        printf("Invalid command\n");
    } else {
        int ret = chdir(command[1]);
        if (ret) return 0;
    }
    if (result) {
        char* res = getcwd(curPath, BUFFSIZE);
        if (res == NULL) {
            printf("The file does not exist!\n");
        }
        return result;
    }
    return 0;
}
/* main函数 */
int main() {
    // while循环
    while(1) {
        printf("stash $ ");
        // 输入字符存入buf数组, 如果输入字符数为0, 则跳过此次循环
        if (get_input(buf) == 0)
            continue;
        strcpy(backupBuf, buf);
        parse(buf);
        runCommand(argc, argv);
        argc = 0;
    }
}
