#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"




int
main(int argc, char **argv)
{
struct TimeVariables t[10];

int turnAroundTimeAvg = 0;
int WaitingTimeAvg = 0;
uint cbtAvg = 0;

changePolicy(2);
for (int id=0; id<10; id++){
    int f = fork();     
    if (f == 0) {
        int pid = getpid();
        
        for (int i = 0; i < 1000; i++)
        {
            printf(1,"%d   [%d]: [%d]\n",id,pid,i);
        }
      
        exit();
    }
    else if(f > 0){
    //   printf(1,"%d",waitForChild(&t[id]));
 // printf(1,"creationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\n",t[id].creationTime,t[id].terminationTime,t[id].sleepingTime,t[id].readyTime);
 
    }
}

    for(int i=0;i<25;i++){

        printf(1,"%d",waitForChild(&t[i]));
    }


    printf(1,"\nQUANTOM : %d\n",QUANTUM);
    for (int i = 0; i < 10; i++)
    {
         printf(1,"\n\nProcess : %d \n->>>>>>>>>>>>>>>>>\ncreationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\nrunningTime :%d\n\n"
         ,i,t[i].creationTime,t[i].terminationTime,t[i].sleepingTime,t[i].readyTime,t[i].runningTime);
         int turnAroundTime = t[i].terminationTime - t[i].creationTime;
         int WaitingTime = t[i].readyTime;
         int cbt = t[i].runningTime;
         turnAroundTimeAvg+=turnAroundTime;
         cbtAvg += cbt;
         WaitingTimeAvg += WaitingTime;
        printf(1,"____________________________________\nTurnAroundTime : %d\nWaitingTime : %d\nCBT : %d\n__________________________\n<----------------------------\n"
        ,turnAroundTime,WaitingTime,cbt);
    }
    turnAroundTimeAvg/=10.0;
    cbtAvg /= 10.0;
    WaitingTimeAvg /= 10.0;
    char str[] = "                                           ";
    printf(1,"Final Results : for quantom %d\n%s -_-_-_-_-_-_-_-_-\n%sTT : %d\n%sWT : %d\n%sCBT : %d\n%s-_-_-_-_-_-_-_-_-\n_______________________________________________________________________________________________"
,QUANTUM,str,str,turnAroundTimeAvg,str,WaitingTimeAvg,str,cbtAvg,str);


exit();
}