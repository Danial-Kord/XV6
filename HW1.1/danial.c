#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>


int main()
{
    int pipeArray[2];

    char str[100];


    if (pipe(pipeArray)==-1)
    {
      printf("%s\n","pipe error");
      return -1;
    }

    scanf("%s", str);
    pid_t fork1 = fork();

    if (fork1 < 0)
    {
      printf("%s\n","fork error");
        return -1;
    }

    else if (fork1 == 0)
    {
      printf("%s\n", "child is writing");

        close(pipeArray[0]);
        write(pipeArray[1], str, strlen(str)+1);
        printf("%s%s\n", "child wrote! : ",str);
        close(pipeArray[1]);
    }


    else
    {
      printf("%s\n", "dad is waiting");
        close(pipeArray[1]);
        char recievedStr[100];
        read(pipeArray[0], recievedStr, 100);
        printf("%s\n", "dad recieved! : ");
        printf("%s\n",recievedStr );
        close(pipeArray[0]);


        exit(0);
    }
}
