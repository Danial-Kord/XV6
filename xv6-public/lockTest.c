#include "types.h"
#include "stat.h"
#include "user.h"

#define count 10
int
main(int argc, char **argv)
{

printf(1,"now entered the test lock function\n");

int pid;
ticketlockinit();

pid = fork();
for (int i = 1; i < count; i++)
{
    if(pid < 0){
        printf(1,"fork fail");
        exit();
    }
    else if(pid > 0)
    {
        pid = fork();
    }
}
    if(pid < 0){
        printf(1,"fork fail");
        exit();
    }   
    else if(pid == 0){
         printf(1,"\nlocking\n");
         ticketlockTest();

        exit();
    } 
    else
    {
       for (int i = 0; i < count+1; i++){
           wait();
       } 
       int out = ticketlockTest();
       printf(1,"\nfinished -> ticket : %d",out-1);
       exit();
    }
    




}