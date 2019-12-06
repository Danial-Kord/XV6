#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "PIDsaver.c"
// #include "syscall.h"

extern int MinegetCounter(int);

int
sys_getpid(void)
{
  cprintf("returning PID proccess...\n");
  return myproc()->pid;
}

int
sys_fork(void)
{
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
  cprintf("exiting proccess...\n");
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  cprintf("waiting proccess...\n");
  return wait();
}

int
sys_kill(void)
{
  cprintf("killing proccess...\n");
  int pid;
  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}



int
sys_sbrk(void)
{
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
  cprintf("uptime proccess...\n");

  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}



int
sys_getChildren(){
  cprintf("getChildren proccess...\n");

return getChildrenPIDs(sys_getpid());
}



int
sys_getCount(int in){
//  char **a;
  cprintf("getCount proccess...\n");
  cprintf("wtf : %d\n",in);
  //int in=0;
  //fetchstr(in, a);

  return MinegetCounter(in);
}
