#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
 
void execPipe(char*argv[],int argc);
 
 
#define MAXARGS 10
#define MAXLINE 100
 
 //从终端中读入字符串用buf保存起来
int getcmd(char *buf, int nbuf)
{
    //打印@而不是$
    fprintf(2, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if (buf[0] == 0) // EOF
        return -1;
    return 0;
}

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(-1);
}

//可以提示错误的fork
int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}


//需要在识别中跳过的字符集 
char skipchar[] = " \t\r\n\v";
 
void setargv(char *cmd, char* argv[],int* argc)
{

    int i = 0; // 表示第i个word`
    
    int j = 0;
    for (; cmd[j] != '\n' && cmd[j] != '\0'; j++)
    {
        // 每一轮循环都是找到输入的命令中的一个word
        
        // 跳过之前的空格
        while (strchr(skipchar,cmd[j])){
            j++;
        }

        //记录一个单词的首地址
        argv[i++]=cmd+j;

        // 只要不是空格，就j++,找到下一个空格
        while (strchr(skipchar,cmd[j])==0){
            j++;
        }
        cmd[j]='\0';
    }

    //设为0，为了符合exec的要求
    argv[i]=0;

    //参数数量
    *argc=i;
}
 
void runcmd(char*argv[],int argc)
{

    //遇到pipe的情况
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"|")){
            execPipe(argv,argc);
        }
    }


    //需要重定向的情况
    for(int i=1;i<argc;i++){
        // 输出重定向
        if(!strcmp(argv[i],">")){

            //关闭标准输出文件描述符
            close(1);

            //下一个参数即为需要重定向的文件
            open(argv[i+1],O_CREATE|O_WRONLY);

            //设为0，为了符合exec的要求
            argv[i]=0;

        }

        // 输入重定向
        if(!strcmp(argv[i],"<")){

            //关闭标准输入文件描述符
            close(0);

            open(argv[i+1],O_RDONLY);

            //设为0，为了符合exec的要求
            argv[i]=0;
        }
    }

    //执行命令，第一个参数为需要执行的命令
    exec(argv[0], argv);
}
 
 //参照sh.c 100行
void execPipe(char*argv[],int argc){
    int i=0;
    // 首先找到命令中的"|",然后把他换成'\0'
    // 从前到后，找到第一个就停止，后面都递归调用
    for(;i<argc;i++){
        if(!strcmp(argv[i],"|")){
            argv[i]="\0";
            break;
        }
    }

    int fd[2];
    if(pipe(fd)<0)
        panic("pipe");
    if(fork1()==0){
        //子进程 执行左边的命令 把自己的标准输出关闭
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv,i);
    }else{
        //父进程 执行右边的命令 把自己的标准输入关闭
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv+i+1,argc-i-1);
    }
    close(fd[0]);
    close(fd[1]);
    wait(0);
    wait(0);
}



int main()
{
    char buf[MAXLINE];
    // Read and run input commands.
    while (getcmd(buf, sizeof(buf)) >= 0)
    {
 
        if (fork1() == 0)
        {
            //指向各个参数在cmd中的位置的指针
            char* argv[MAXARGS];

            //初始化参数的数量为-1
            int argc=-1;

            //获取参数的位置并设置参数在argv中的位置
            setargv(buf, argv,&argc);

            //执行命令
            runcmd(argv,argc);
        }

        //等待子进程结束
        wait(0);
    }
 
    exit(0);
}