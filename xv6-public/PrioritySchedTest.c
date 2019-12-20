#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"




int
main(int argc, char **argv)
{
struct TimeVariables t[25] ;
int pids[25];


changePolicy(1);

int f =0;
int id;
for ( id=0; id<25; id++) {
     f = fork();
    if (f == 0) {
        int pid = getpid();
        changePriority(pid,5-id/5);
        break;
       // exit();
    }

}

    if(f == 0){
        for (int i = 0; i < 500; i++)
        {
            printf(1,"[%d]: [%d]\n",i,id);
        }
        
    }
    else{
int turnAroundTimeAvg = 0;
int WaitingTimeAvg = 0;
uint cbtAvg = 0;
    for ( id=0; id<25; id++){
     pids[id] = waitForChild(&t[id]);
    }
 // print(1,"creationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\n",t[id].creationTime,t[id].terminationTime,t[id].sleepingTime,t[id].readyTime);

    for(int i=0;i<25;i++){
        int index = i;
        int min = pids[i];
        for (int j = i; j < 25; j++)
        {
            if(pids[j] < min){
                min = pids[j];
                index = j;
            }
        }
         struct TimeVariables temp;
        temp = t[i];
        t[i] = t[index];
        t[index] = temp;
        int tt =pids[i];
        pids[i] = pids[index];
        pids[index] = tt;
    }


    printf(1,"creationTime:\t terminationTime:\t sleepiing:\treadytime:\t\n");
    for (int i = 0; i < 25; i++)
    {
         printf(1,"\n\nProcess : %d \n->>>>>>>>>>>>>>>>>\npid : %d\ncreationTime : %d\nterminationTime : %d\nsleepiing : %d\nreadytime :%d\n\n"
         ,i,pids[i],t[i].creationTime,t[i].terminationTime,t[i].sleepingTime,t[i].readyTime);
         int turnAroundTime = t[i].terminationTime - t[i].creationTime;
         int WaitingTime = t[i].readyTime;
         int cbt = t[i].runningTime ;
         turnAroundTimeAvg+=turnAroundTime;
         cbtAvg += cbt;
         WaitingTimeAvg += WaitingTime;
        printf(1,"____________________________________\nTurnAroundTime : %d\nWaitingTime : %d\nCBT : %d\n__________________________\n<----------------------------\n"
        ,turnAroundTime,WaitingTime,cbt);
    }


    for(int i=0;i<5;i++){

         int turnAroundTime = 0;
         int WaitingTime = 0;
         int cbt = 0;
         printf(1,"___________________________________________________\nsame priority of : %d \n",5-i);
        for(int j=0;j<5;j++){
          turnAroundTime += t[i*5+j].terminationTime - t[i*5+j].creationTime;
          WaitingTime += t[i*5+j].readyTime;
          cbt += t[i*5+j].runningTime;    
        }
        turnAroundTime/=5;
        WaitingTime/=5;
        cbt/=5;
         printf(1,"\nTurnAroundTime : %d\nWaitingTime : %d\nCBT : %d\n__________________________\n<----------------------------\n"
        ,turnAroundTime,WaitingTime,cbt);
    }



    turnAroundTimeAvg/=25.0;
    cbtAvg /= 25.0;
    WaitingTimeAvg /= 25.0;
    printf(1,"-_-_-_-_-_-_-_-_-\nTT : %d\nWT : %d\nCBT : %d\n-_-_-_-_-_-_-_-_-\n"
,turnAroundTimeAvg,WaitingTimeAvg,cbtAvg);

    }
exit();
}