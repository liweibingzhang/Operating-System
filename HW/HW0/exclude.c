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
        int num1,num2 = 0;
        int cnt = 0;
        int bytes = read(fd1, buffer, BUFFER_SIZE);
        for (int i = 0; i < BUFFER_SIZE;i++){
            if(buffer[i]=='\n')
                cnt++;
            if(cnt==(line_num-1)){
                num1 = i+1;  //record the position of the ch after the'\n' of(line_num-1)
                break;
            }
        }
        cnt = 0;
        for (int i = 0; i < BUFFER_SIZE;i++){
            if(buffer[i]=='\n')
                cnt++;
            if(cnt==(line_num)){
                num2 = i+1; //record the position of the ch after the'\n' of(line_num)
                break;
            }
        }
        for (int i = num1; i < BUFFER_SIZE;i++){
            buffer[i] = buffer[num2];
            num2++;
        }
            if (bytes == -1)
            {
                perror("Read file");
                exit(3);
            }
        if(line_num==1){
            for (int i = 0; i < BUFFER_SIZE;i++){
                buffer[i] = buffer[i + 1];
            }
        }
        printf("%s", buffer);
        // 写到标准输出
        // 文件描述符0代表标准输出

        int ret = write(fd2, buffer, my_strlen(buffer));
        if(ret == -1){      	
            perror("Write stdout");
            exit(4);
        }
    close(fd1);
    close(fd2);
    return 0;
}
