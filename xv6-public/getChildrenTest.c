#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char **argv)
{

printf(1,"now entered the test children PID test function\n");
printf(1,"%d\n",argv[1][0]);
for (int id=0; id<5; id++) {
    if ((fork()) == 0) {
        exit();
    }
    else{
      wait();
    }
}


int lengh =0;
int i;
for (i = 0; argv[1][i] >=48 && argv[1][i] <=57; i++) {
}
lengh = i;

int a =0;
int index =1;
for (int i = lengh-1; i >=0; i--) {
    a += index*(argv[1][i]-48);
    index*=10;
}



int out = getChildren(a);
printf(1,"the number of all child PIDs : \n%d",out);
exit();
}