#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 102400
    int my_strlen(const char *str)
{
	int result=0;
	while (*str)
	{
		result++;
		str++;
	}
	return result;
}
int my_atoi(char *str)
{
    int result = 0;
    int flag = 1;
    while (*str != '\0')
    {
        if (*str >= '0' && *str <= '9')
        {
            result = result * 10 + ( *str - '0' );
        }
        else
        {
            break;
        }
 
        str++;
    }
    return result * flag;
}
int main(int argc, char *argv[])
{

    //检查参数
    if (argc != 4) {
        perror("Wrong input");
        exit(1);
    }

    // 打开文件
    int fd1 = open(argv[1], O_RDONLY);
    int fd2 = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC,0777);
    int line_num = my_atoi(argv[3]);
    if (fd1 == -1) {
        perror("Open file");
	    exit(2);
    }
    // 循环输出        
        char buffer[BUFFER_SIZE];
        // 读文件
        int num1=0,num2 = 0;
        int cnt = 0;
        int bytes = read(fd1, buffer, BUFFER_SIZE);
        int flag = 0;
        int total_line = 0;
        int i;
        for (int i = 0; i < my_strlen(buffer);i++){
            if(buffer[i]=='\n')
            total_line++;
        }
        if(total_line<line_num) 
            perror("Write stdout");
        while(1){
            if (cnt < line_num - 1)         
            {
                for (i = 0; i < bytes; i++)
                {
                    if (buffer[i] == '\n')
                    {
                        cnt++;
                    }
                    if (cnt == line_num - 1){
                        num1 = i;
                        break;
                    }
                }
                if(cnt==line_num-1){
                    write(fd2, buffer, num1);
                    continue;
                }
                else{
                    write(fd2, buffer, bytes);
                    bytes = read(fd1, buffer, BUFFER_SIZE);
                }
            }
            else if (cnt == line_num - 1)
            {
                for (i = num1 + 1; i < bytes; i++)
                {
                    if(buffer[i]=='\n')
                        cnt++;
                    if (cnt == line_num)
                    {
                        num2 = i;
                        break;
                    }
                }
                if (cnt == line_num)
                {
                    write(fd2, &buffer[num2], bytes - num2);
                    if (bytes == BUFFER_SIZE){
                        bytes = read(fd1, buffer, BUFFER_SIZE);
                        continue;
                    }
                    else
                        break;
                }
                else if (cnt == line_num - 1)
                {
                    if (bytes == BUFFER_SIZE)
                    {
                        bytes = read(fd1, buffer, BUFFER_SIZE);
                        continue;
                    }
                    else{
                        break;
                    }
                }
            }
            else if (cnt >= line_num)
            {
                write(fd2, buffer, bytes);
                if(bytes == BUFFER_SIZE)
                    bytes = read(fd1, buffer, BUFFER_SIZE);
                else
                    break;
            }
        }
        
        //printf("%s", buffer);
        // 写到标准输出
        // 文件描述符0代表标准输出

        // int ret = write(fd2, buffer, count);
        // if(ret == -1){      	
        //     perror("Write stdout");
        //     exit(4);
        // }
    close(fd1);
    close(fd2);
    return 0;
}
