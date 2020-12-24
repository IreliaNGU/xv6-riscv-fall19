#include "kernel/types.h"

#include "user/user.h"



void func(int *input, int num){

	//当输入的数字为1时

	if(num == 1){

		printf("prime %d\n", *input);

		return;

	}
	

	int p[2],i;

	int prime = *input;

	int temp;

	//输出当前最小的质数

	printf("prime %d\n", prime);

	pipe(p);

	//第一个输入管道

    if(fork() == 0){

        for(i = 0; i < num; i++){

			//全部数字写入

            temp = *(input + i);

			write(p[1], (char *)(&temp), 4);

		}

        exit(0);

    }

	//关闭写管道

	close(p[1]);

	//读出管道

	if(fork() == 0){

		int counter = 0;

		char buffer[4];

		while(read(p[0], buffer, 4) != 0){

			temp = *((int *)buffer);

			//判断是否能被当前最小的质数除

			if(temp % prime != 0){

				//覆盖之前的数组单元

				*input = temp;

				input += 1;

				counter++;

			}

		}

		//递归调用，长度为筛选后的长度

		func(input - counter, counter);

		exit(0);

    }

	//等待两个子进程结束

	wait(0);

	wait(0);

}



int main(){

    int input[34];

	int i = 0;

	for(; i < 34; i++){

		input[i] = i+2;

	}

	func(input, 34);

    exit(0);

}