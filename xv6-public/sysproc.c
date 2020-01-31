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



int
sys_getpid(void)
{
 
  
  return myproc()->pid;
}

int
sys_fork(void)
{
  

  
  int dadPID = sys_getpid();
  addPID(dadPID);
  cprintf("id dad : %d\n",dadPID);
  int output = fork();
  // if(output == 0){
  //   exit();
  // }
    cprintf("id child : %d\n",output);
    addChildPID(dadPID,output);
  //  wait();
    return output;
}

int
sys_exit(void)
{
 

 
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  

  return wait();
}
int
sys_waitForChild(void){

    struct TimeVariables *timeVariables;
    if(argptr (0 , (void*)&timeVariables ,sizeof(*timeVariables)) < 0)
        return -1;
  cprintf("%d",timeVariables->readyTime);
  return waitForChild(timeVariables);
}
int
sys_kill(void)
{
  

 
  int pid;
  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}



int
sys_sbrk(void)
{
  

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
  



  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


int sys_currentStatus(){
  return cps();
}


int
sys_changePriority (void)
{
  int pid, pr;
  if(argint(0, &pid) < 0)
    return -1;
  if(argint(1, &pr) < 0)
    return -1;
   
  chpr ( pid, pr );
  return 1;
}


int
sys_changePolicy (void)
{
  int in;
  if(argint(0, &in) < 0)
    return -1;

  return chsp(in);
}


int
sys_getChildren(int temp_pid){
 
  argint(0, &temp_pid);


return getChildrenPIDs(temp_pid);
}


int
sys_getCount(int in){
 

//  char **a;
argint(0, &in);

 
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

int
sys_ticketlockinit(){
  initTicket();
  return 0;
}


int
sys_ticketlockTest(){
  return ticketLock();
}
int
sys_rwinit(){
  rwinit();
return 0;
}

int
sys_rwtest(int in){
  argint(0, &in);
  return  rwTest(in);

}