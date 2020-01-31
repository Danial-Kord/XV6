#include "types.h"
#include "stat.h"
#include "user.h"


void testRw(int * pattern,int size);

int
main()
{
char argv[2][100];
printf(1,"now entered the test rw function\n");

int size = read(0,argv[0],sizeof(argv[0]));

int pattern[100];

for (int i = 0; i < size-1; i++)
{
    printf(1,"%c",argv[0][i]);
    if(argv[0][i] == '0'){
        pattern[i] = 0;
    }
    else if(argv[0][i] == '1')
    {
        pattern[i] = 1;
    }
    else
    {
        printf(1,"\nwrong input!\n");
        exit();
    }
    
}
printf(1,"\n");
testRw(pattern,size-1);
exit();
return 0;
}



void 
testRw(int * pattern,int size){
int pid=-1,i=0;
rwinit();

printf(1,"\n\n\n %d \n\n\n",size);

for (int i = 1; i < size; i++)
{
    printf(1,"%d\n\n",pattern[i]);
}
    pid = fork();
for ( i = 1; i < size; i++)
{



    if(pid < 0){
        printf(1,"fork fail");
        exit();
    }
    else if(pid > 0)
    {
        pid = fork();
    }
    else
    {
        break;
    }
    
}
    if(pid < 0){
        printf(1,"fork fail");
        exit();
    }   
    else if(pid == 0){

         printf(1,"child adding to shared data \n");
         int shareData = rwtest(pattern[i]);
        if(pattern[i] == 0){
            printf(1,"pid : %d read from sharedata : %d\n",getpid(),shareData);
        }
        else{
            printf(1,"pid : %d writed from sharedata : %d\n",getpid(),shareData);

        }

        exit();
    } 
    else
    {
       for (int i = 0; i < size; i++){
           wait();
       } 
       int out = rwtest(0);
       printf(1,"\nfinished \n");
       printf(1,"last amount shared data is : %d",out);
       exit();
    }
    
}