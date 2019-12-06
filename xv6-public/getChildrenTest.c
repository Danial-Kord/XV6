#include "types.h"
#include "stat.h"
#include "user.h"
int index=0;
int
main(void)
{
  fork();
int i = getChildren();
printf(1,"%d",i);
index++;
exit();
}
