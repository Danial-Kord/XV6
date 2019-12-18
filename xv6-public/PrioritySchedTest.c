#include "types.h"
#include "stat.h"
#include "user.h"
//#include "proc.h"




int
main(int argc, char **argv)
{
struct TimeVariables t[25] ;

int turnAroundTimeAvg = 0;
int WaitingTimeAvg = 0;
uint cbtAvg = 0;


for (int id=0; id<25; id++) {
    int f = fork();
    if (f == 0) {
        int pid = getpid();
        
        for (int i = 0; i < 1000; i++)
        {
            printf(1,"[%d]: [%d]\n",id,id,i);
        }
      
        exit();
    }
    else if(f > 0){
      printf(1,"%d",waitForChild(&t[id]));
 // printf(1,"creationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\n",t[id].creationTime,t[id].terminationTime,t[id].sleepingTime,t[id].readyTime);
 
    }
}
    printf(1,"creationTime:\t terminationTime:\t sleepiing:\treadytime:\t\n");
    for (int i = 0; i < 25; i++)
    {
         printf(1,"\n\nProcess : %d \n->>>>>>>>>>>>>>>>>\ncreationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\n\n"
         ,i,t[i].creationTime,t[i].terminationTime,t[i].sleepingTime,t[i].readyTime);
         int turnAroundTime = t[i].terminationTime - t[i].creationTime;
         int WaitingTime = t[i].readyTime;
         int cbt = turnAroundTime - WaitingTime - t[i].sleepingTime;
         turnAroundTimeAvg+=turnAroundTime;
         cbtAvg += cbt;
         WaitingTimeAvg += WaitingTime;
        printf(1,"____________________________________\nTurnAroundTime : %d\nWaitingTime : %d\nCBT : %d\n__________________________\n<----------------------------\n"
        ,turnAroundTime,WaitingTime,cbt);
    }
    turnAroundTimeAvg/=10.0;
    cbtAvg /= 10.0;
    WaitingTimeAvg /= 10.0;
    printf(1,"-_-_-_-_-_-_-_-_-\nTurnAroundTimeAvg : %d\nWaitingTimeAvg : %d\nCBTAvg : %d\n-_-_-_-_-_-_-_-_-\n"
,turnAroundTimeAvg,WaitingTimeAvg,cbtAvg);


exit();
}