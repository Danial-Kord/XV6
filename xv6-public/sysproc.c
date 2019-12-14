#include "types.h"
#include "stat.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "PIDsaver.c"
// #include "getCountTest.c"
// #include "syscall.h"

extern int MinegetCounter(int);

void updateCounter(int in){
if(in <= 23 && in>=1){
  myproc()->count[in-1]++;
  cprintf("updated data for pid :%d  count sysnumber %d : %d\n",myproc()->pid,in,myproc()->count[in-1]);
}
}

int
sys_getpid(void)
{
  updateCounter(11);
  cprintf("returning PID proccess...\n");
  return myproc()->pid;
}

int
sys_fork(void)
{
  updateCounter(1);

  cprintf("forking proccess...\n");
  int dadPID = sys_getpid();
  addPID(dadPID);
  cprintf("id dad : %d\n",dadPID);
  int output = fork();
  if(output == 0){
    exit();
  }
    cprintf("id child : %d\n",output);
    addChildPID(dadPID,output);
    wait();
    return output;
}

int
sys_exit(void)
{
  updateCounter(2);

  cprintf("exiting proccess...\n");
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  updateCounter(3);

  cprintf("waiting proccess...\n");
  return wait();
}

int
sys_kill(void)
{
  updateCounter(6);

  cprintf("killing proccess...\n");
  int pid;
  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}



int
sys_sbrk(void)
{
  updateCounter(12);

  cprintf("sbrk proccess...\n");
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  updateCounter(13);

  cprintf("sleep proccess...\n");

  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  updateCounter(14);

  cprintf("uptime proccess...\n");

  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}



int
sys_getChildren(){
  updateCounter(22);

  cprintf("getChildren proccess...\n");

return getChildrenPIDs(sys_getpid());
}


int
sys_getCount(int in){
  updateCounter(23);

//  char **a;
argint(0, &in);

  cprintf("getCount proccess...\n");
  //cprintf("wtf : %d\n",in);
  //int in=0;
  //fetchstr(in, a);

  //return MinegetCounter(in);
  if(in <= 23 && in>=1){
    
    cprintf("my pid is : %d\n",myproc()->pid);
     return  myproc()->count[in-1];
  }
  return -1;
}
