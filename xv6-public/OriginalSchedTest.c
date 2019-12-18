#include "types.h"
#include "stat.h"
#include "user.h"
//#include "proc.h"




int
main(int argc, char **argv)
{
struct TimeVariables t[10] ;
int pids[10];
float turnAroundTimeAvg = 0;
float WaitingTimeAvg = 0;
float cbtAvg = 0;


for (int id=0; id<10; id++) {
    int f = fork();
    if (f == 0) {
        int pid = getpid();
        pids[id] = pid;
        for (int i = 0; i < 1000; i++)
        {
            printf(1,"%d   [%d]: [%d]\n",id,pid,i);
        }
        exit();
    }
    else if(f > 0){
      printf(1,"%d",waitForChild(&t[id]));
 // printf(1,"creationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\n",t[id].creationTime,t[id].terminationTime,t[id].sleepingTime,t[id].readyTime);
 
    }
}
    printf(1,"creationTime:\t terminationTime:\t sleepiing:\treadytime:\t\n");
    for (int i = 0; i < 10; i++)
    {
         printf(1,"Process %d ->>>>>>>>>>>>>>>>>\ncreationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\n\n"
         ,pids[i],t[i].creationTime,t[i].terminationTime,t[i].sleepingTime,t[i].readyTime);
         int turnAroundTime = t[i].terminationTime - t[i].creationTime;
         int WaitingTime = t[i].readyTime;
         int cbt = turnAroundTime - WaitingTime - t[i].sleepingTime;
         turnAroundTimeAvg+=turnAroundTime;
         cbtAvg += cbt;
         WaitingTimeAvg += WaitingTime;
        printf(1,"____________________________________\nTurnAroundTime : %d\nWaitingTime : %d\nCBT : %d\n__________________________\n"
        ,turnAroundTime,WaitingTime,cbt);
    }
    turnAroundTimeAvg/=10.0;
    cbtAvg /= 10.0;
    WaitingTimeAvg /= 10.0;
    printf(1,"-_-_-_-_-_-_-_-_-\nTurnAroundTimeAvg : %f\nWaitingTimeAvg : %f\nCBTAvg : %f\n-_-_-_-_-_-_-_-_-\n"
,turnAroundTimeAvg,WaitingTimeAvg,cbtAvg);


exit();
}