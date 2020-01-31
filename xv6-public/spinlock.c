#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;

}
void
initTicketLock(struct ticketlock * lk){
  lk->ticket=0;
  lk->turn = 0;
  // for (int i = 0; i < 20; i++)
  // {
  //   lk->proc[i].pid = 999;
  // }
  
}
// Acquire the lock.
// Loops (spins) until the lock is acquired.
// Holding a lock for a long time may cause
// other CPUs to waste time spinning to acquire it.
void
acquire(struct spinlock *lk)
{
  pushcli(); // disable interrupts to avoid deadlock.
  if(holding(lk))
    panic("acquire");

  // The xchg is atomic.
  while(xchg(&lk->locked, 1) != 0)
    ;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen after the lock is acquired.
  __sync_synchronize();

  // Record info about lock acquisition for debugging.
  lk->cpu = mycpu();
  getcallerpcs(&lk, lk->pcs);
}




// Release the lock.
void
release(struct spinlock *lk)
{
  if(!holding(lk))
    panic("release");

  lk->pcs[0] = 0;
  lk->cpu = 0;

  // Tell the C compiler and the processor to not move loads or stores
  // past this point, to ensure that all the stores in the critical
  // section are visible to other cores before the lock is released.
  // Both the C compiler and the hardware may re-order loads and
  // stores; __sync_synchronize() tells them both not to.
  __sync_synchronize();

  // Release the lock, equivalent to lk->locked = 0.
  // This code can't use a C assignment, since it might
  // not be atomic. A real OS would use C atomics here.
  asm volatile("movl $0, %0" : "+m" (lk->locked) : );

  popcli();
}




// Record the current call stack in pcs[] by following the %ebp chain.
void
getcallerpcs(void *v, uint pcs[])
{
  uint *ebp;
  int i;

  ebp = (uint*)v - 2;
  for(i = 0; i < 10; i++){
    if(ebp == 0 || ebp < (uint*)KERNBASE || ebp == (uint*)0xffffffff)
      break;
    pcs[i] = ebp[1];     // saved %eip
    ebp = (uint*)ebp[0]; // saved %ebp
  }
  for(; i < 10; i++)
    pcs[i] = 0;
}

// Check whether this cpu is holding the lock.
int
holding(struct spinlock *lock)
{
  int r;
  pushcli();
  r = lock->locked && lock->cpu == mycpu();
  popcli();
  return r;
}


// Pushcli/popcli are like cli/sti except that they are matched:
// it takes two popcli to undo two pushcli.  Also, if interrupts
// are off, then pushcli, popcli leaves them off.

void
pushcli(void)
{
  int eflags;

  eflags = readeflags();
  cli();
  if(mycpu()->ncli == 0)
    mycpu()->intena = eflags & FL_IF;
  mycpu()->ncli += 1;
}

void
popcli(void)
{
  if(readeflags()&FL_IF)
    panic("popcli - interruptible");
  if(--mycpu()->ncli < 0)
    panic("popcli");
  if(mycpu()->ncli == 0 && mycpu()->intena)
    sti();
}




void
releaseTicketLock(struct ticketlock * lk)
{
  pushcli(); 
  cprintf("releasing\n");
  int i = releaseNext(lk);
  popcli();
  if(i)
  yield();
}
int
acquireTicketLock(struct ticketlock *lk){

  
  pushcli(); 
   
  
  if(lk->ticket == lk->turn){
    cprintf("no one holding lock\n");
  }
  else
  {
  block(myproc()->pid);
  }
    fetch_and_add(&lk->ticket,1);
    cprintf("fetched and added  %d\n",lk->ticket);

    cprintf("adding ticket\n");
    addNewBlock(myproc(),lk);
    cprintf("added ticket \n");
    popcli();
  return lk->ticket;
}

/*
void
initlock_t(struct ticketlock *lk, char *name)
{
  lk->name = name;
  lk->proc = 0;
  lk->ticket = 0;
  lk->turn = 0;
}

// Acquire the lock.
void
acquire_t(struct ticketlock *lk)
{
  uint ticket;
  //pushcli(); // disable interrupts to avoid deadlock.
  if(holding_t(lk))
    panic("acquire");

  ticket = fetch_and_add(&lk->ticket, 1);
  while(lk->turn != ticket)
    givepriority(lk->proc);

  // Record info about lock acquisition for debugging.
  lk->cpu = cpu;
  lk->proc = proc;
  getcallerpcs(&lk, lk->pcs);
}

// Release the lock.
void
release_t(struct ticketlock *lk)
{
  if(!holding_t(lk))
    panic("release");

  lk->pcs[0] = 0;
  lk->proc = 0;
  lk->cpu = 0;

  lk->turn++; //fetch_and_add(&lk->turn, 1);
  wakeup(lk);
  resetpriority();

  //popcli();
}

// Check whether this cpu is holding the lock.
int
holding_t(struct ticketlock *lock)
{
  return (lock->ticket != lock->turn) && (lock->proc == proc);
}*/ 