#include "types.h"
#include "stat.h"
#include "user.h"
//#include "proc.h"




int
main(int argc, char **argv)
{
struct TimeVariables *t = malloc(sizeof(struct TimeVariables));
t->readyTime = 110;

for (int id=0; id<1; id++) {
    int f = fork();
    if (f == 0) {
        int pid = getpid();
        for (int i = 0; i < 1000; i++)
        {
            printf(1,"%d   [%d]: [%d]\n",id,pid,i);
        }
        //exit();
    }
    else{
      printf(1,"%d",waitForChild(t));
    }
}
    printf(1,"creationTime:\t terminationTime:\t sleepiing:\treadytime:\t\n");
    for (int i = 0; i < 1; i++)
    {
        printf(1,"\t%d\t%d\t%d\t%d\n",t->creationTime,t->terminationTime,t->sleepingTime,t->readyTime);
    }
    


exit();
}