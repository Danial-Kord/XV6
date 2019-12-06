#include "types.h"
#include "stat.h"
#include "user.h"
int index=0;
int
main(void)
{

printf(1,"now entered the test function\n");
for (int id=0; id<5; id++) {
    if ((fork()) == 0) {
        exit();
    }
    else{
      wait();
    }
}



int i = getChildren();
printf(1,"the number of all child PIDs : \n%d",i);
index++;
exit();
}
