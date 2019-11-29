#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>

int isEven(char in){
  if(in >= 48 && in <= 59){
    if(in%2 ==0)
        return in-48;
    else
        return 0;
  }
  return -1;
}
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

        int evenSum = 0;
        for(int i=0;i<100;i++){
          if(recievedStr[i] == NULL)
              break;
          int out = isEven(recievedStr[i]);
            if(out != -1){
              evenSum += out;
            }
            else{
              printf("%s\n","wrong input!" );
              return -1;
            }
        }
        printf("%s : %d\n","The sum of even digits in number is",evenSum);


        exit(0);
    }
}
