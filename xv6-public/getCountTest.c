#include "types.h"
#include "stat.h"
#include "user.h"

// int number(char in){
//   return in
// }


int
main(int argc, char **argv)
{


printf(1,"now entered the test count test function\n");
printf(1,"%d\n",argv[1][0] );
int lengh =0;
int i;
for (i = 0; argv[1][i] >=48 && argv[1][i] <=57; i++) {
  i++;
}
lengh = i;

int a =0;
int index =1;
for (int i = lengh-1; i >=0; i--) {
    a += index*(argv[1][i]-48);
  //  printf(1,"%d\n",argv[1][i]-48);
    index*=10;
}





 i = getCount(a);
printf(1,"the number of used of systemcall number %d is : \n%d",a,i);
exit();
return 0;
}
