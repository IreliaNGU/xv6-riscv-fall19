#include "kernel/types.h"
#include "user/user.h"

int main(int argn, char *argv[]){
	if(argn != 2){
		fprintf(2, "Please input 2 arguments.argument number wrong!\n");
		exit(1);
	}

	//字符转数字
	int sleepTime = atoi(argv[1]);

	printf("Sleep %d\n",sleepTime);

	sleep(sleepTime);

	exit(0);
}