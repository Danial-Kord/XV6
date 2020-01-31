#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"



struct {
  struct spinlock lock;
  struct ticketlock ticketLock;
  struct proc proc[NPROC];
} ptable;

struct rw rw;



static struct proc *initproc;


int nextpid = 1;
int currentPolicy=0;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
  initTicketLock(&ptable.ticketLock);

}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;

  //DanialKm
  
  
  p->timeVariables.creationTime = ticks;
  p->timeVariables.readyTime = 0;
  p->timeVariables.runningTime = 0;
  p->timeVariables.sleepingTime = 0;
  p->tickcounter = 0;
  p->calculated_priority = 0;
  
  //DanialKm choosing the min priority
  //TODO
  struct proc* p1;
  int minPriority=0;;
  p1 = ptable.proc;
  
    for(p1 = ptable.proc; p1 < &ptable.proc[NPROC]; p1++){
    if(p1->state != RUNNABLE && p1->state != SLEEPING)
      continue;
    else if(p1->pid != p->pid){
        minPriority = p1->calculated_priority;
        break;
    }
  }
      
  for(p1 = ptable.proc; p1 < &ptable.proc[NPROC]; p1++){
  if(p1->state != RUNNABLE&& p1->state != SLEEPING) //choosing the min priority among the sleeping or runnable processes...it can change to just check just the runnables
    continue;
  else if(p1->calculated_priority < minPriority && p1->pid != p->pid)
  minPriority = p1->calculated_priority;
  }
  cprintf("choosen priority for new process: %d",minPriority);
  p->calculated_priority = minPriority;
  
    for(p1 = ptable.proc; p1 < &ptable.proc[NPROC]; p1++){
  if(p1->state != RUNNABLE&& p1->state != SLEEPING) //choosing the min priority among the sleeping or runnable processes...it can change to just check just the runnables
    continue;
  else 
  p1->calculated_priority-=minPriority;
  }
  p->priority = 5;//default lowest

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);
  if(p->state != WAITING)
  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }
  
    //releaseNext();
  
  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  //TODO
  curproc->timeVariables.terminationTime = ticks;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        //DanialKm
       // p->timeVariables.terminationTime = ticks;
      
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}


//DanialKm
int
waitForChild(struct TimeVariables *timeVariables)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        //DanialKm
        //  timeVariables->readyTime = p->timeVariables.readyTime;
        //  timeVariables->creationTime = p->timeVariables.creationTime;
        //  timeVariables->sleepingTime = p->timeVariables.sleepingTime;
        //  timeVariables->terminationTime = p->timeVariables.terminationTime;
        cprintf("wtffffffffff\n");
        *timeVariables = p->timeVariables;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      cprintf(":(((((((((\n");
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}


//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.


//DanialKm
void
scheduler(void)
{
  struct proc *p;

  

  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    


   
    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      
      if(p->state != RUNNABLE){
        continue;
      }
      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
    

     
      // DanialKm : choose one with highest priority
      if(currentPolicy == 1){
      struct proc *p1;
      struct proc *highP;
      highP = p;
       //cprintf("Process %s with pid %d and priority : %d running\n", p->name, p->pid,p->calculated_priority);
      for(p1 = ptable.proc; p1 < &ptable.proc[NPROC]; p1++){
        if(p1->state != RUNNABLE)
          continue;
        if ( highP->calculated_priority > p1->calculated_priority )   // larger value, lower priority 
          highP = p1;
      }
    
      highP->calculated_priority += highP->priority;
      p = highP; 
       //cprintf("choosen -->: Process %s with pid %d and priority : %d running\n", p->name, p->pid,p->calculated_priority);
      }
      
     
      // if(currentPolicy != 2){
      c->proc = p;
      switchuvm(p);
      int i=0;
      if(p->state != WAITING){
         p->state = RUNNING;
          i=1;
      }
      //p->tickcounter = 0;
      // }
      // else if(p->tickcounter >= QUANTUM){
      // c->proc = p;
      // switchuvm(p);
      // p->state = RUNNING;        
      // }
      
      
      
      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
      if(i==1)
      break;
    }
    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  if(myproc()->state != WAITING)
  myproc()->state = RUNNABLE;
  //DanialKm
  //if(myproc()->tickcounter >= QUANTUM)
      myproc()->tickcounter = 0;
  sched();
  release(&ptable.lock);
}


// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from schedulrer.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}




//DanialKm








//change priority
int
chpr( int pid, int priority )
{
  struct proc *p;
  
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid ) {
        p->priority = priority;
        break;
    }
  }
  release(&ptable.lock);
   cprintf("changed priority of %d to : %d\n",pid,priority);
  return pid;
}


int getPolicy(){
  return currentPolicy;
}

//change system polisy
int
chsp(int in){
  if(in >=0 && in <=2){
    currentPolicy = in;
    cprintf("changed policy to num%d",in);
    return 1;
  }
  return -1;
}

//current process status
int
cps()
{
  struct proc *p;
  

    // Loop over process table looking for process with pid.
  acquire(&ptable.lock);
  cprintf("_________________________________\n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if ( p->state == SLEEPING )
        cprintf("%s \t %d  \t SLEEPING \t%d \t%d\t%d \n", p->name, p->pid ,p->calculated_priority,p->timeVariables.creationTime,p->reader);
      else if ( p->state == RUNNING )
        cprintf("%s \t %d  \t RUNNING \t%d \t%d\t%d\n ", p->name, p->pid ,p->calculated_priority,p->timeVariables.creationTime,p->reader);
        else if( p->state == RUNNABLE )
        cprintf("%s \t %d  \t RUNNING \t%d \t%d\t%d \n", p->name, p->pid ,p->calculated_priority,p->timeVariables.creationTime,p->reader);
                else if( p->state == WAITING )
        cprintf("%s \t %d  \t waitning \t%d \t%d reader :\t%d\n ", p->name, p->pid ,p->calculated_priority,p->timeVariables.creationTime,p->reader);
  }
  
  release(&ptable.lock);
  
  return 22;
}

void
updateTableTiming(){
    

    
    struct proc *p;
    // Loop over process table looking for process with pid.
  acquire(&ptable.lock);
  //cprintf("name \t pid \t state \n");
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if ( p->state == SLEEPING )
        p->timeVariables.sleepingTime++;
      else if ( p->state == RUNNABLE)
        p->timeVariables.readyTime++;
      else if ( p->state == RUNNING)
        p->timeVariables.runningTime++;
      
        
  }
  
  release(&ptable.lock);
}

void
block(int pid){
    struct proc *p;
    // Loop over process table looking for process with pid.
  acquire(&ptable.lock);
  //cprintf("name \t pid \t state \n");
  myproc()->state = WAITING;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->state = WAITING; 
      break;
      }
      
  }
  release(&ptable.lock);

}

void
addNewBlock(struct proc* newProc,struct ticketlock* ticketLock){


    acquire(&ptable.lock);
   // struct ticketlock  *ticketlock = &ptable.ticketLock;
    newProc->ticktNumber = ticketLock->ticket;

   
   ticketLock->proc[ticketLock->ticket-1] = newProc;
   ticketLock->proc[ticketLock->ticket] = 0;
  //  for (int i = 0; i < ticketLock->ticket; i++)
  //  {
  //    cprintf("p : %d --> state:\n",ticketLock->proc[i]->pid);
  //  }
   
   // ptable.blocks_t[ptable.maxLock-1].next = 0;

    /*
    struct blocks* blocks = ptable.blocks;
    struct blocks mblock;
    struct blocks* block = &mblock;
    // newProc->ticktNumber = ptable.maxLock;
    int ticket = newProc->ticktNumber;
    block->proc = newProc;
    block->next = 0;
    struct blocks* cur = blocks;
    struct blocks* temp = blocks->next;
    if(blocks != 0){
      if(temp != 0){
         cprintf("here!!!>>>>\n");
        do
        {
          if(ticket < temp->proc->ticktNumber || temp->next != 0){
            block->next = temp;
            cur->next = block;
            break;
        }
          temp = temp->next;
          cur = cur->next;
        }while (temp->next != 0);
      }
      else
      {
        if(blocks->proc->ticktNumber < ticket){
          block->next = blocks;
          blocks->next = 0;
        }
        else
        {
          blocks->next = block;
        }
        
      }
      
    }
    else{
      // ptable.maxLock = 1;
      blocks = block;
    }
    
    cprintf("\n<<<<<here!!!");*/
  
    cprintf("added to array pid : %d ticket %d turn %d \n",newProc->pid,ticketLock->ticket,ticketLock->turn);
    release(&ptable.lock);
    cps();
    
}

int
releaseNext(struct ticketlock *ticketLock){

  
  // struct blocks * first= ptable.blocks;
 // ptable.blocks = ptable.blocks->next;
 
 struct proc *cur = myproc();
//  for (int i = 0; i < ptable.maxLock; i++)
//  {
//    if(cur->pid == ptable.blocks_t[i].proc->pid){
//      int min = 0;
//      for (int j = 1; j < ptable.maxLock; j++)
//      {
//        if(ptable.blocks_t[j].proc->ticktNumber < ptable.blocks_t[min].proc->ticktNumber && i!=j)
//           min =j;
//      }
     
//       ptable.blocks_t[min].proc->state = RUNNABLE;
//       ptable.last++;
//       cprintf("\n\n\n awaked ticket : %d \n\n\n",ptable.blocks_t[min].proc->ticktNumber);
//       break;
//    }
//  }
    //struct ticketlock *ticketLock = &ptable.ticketLock;
    cprintf("cur pid  : %d turn pid : %d\n",cur->pid,ticketLock->proc[ticketLock->turn]->pid);
    // if(cur->pid == ticketLock->proc[ticketLock->turn]->pid){
      ticketLock->turn++;
      
      if(ticketLock->proc[ticketLock->turn] == 0)
      return 0;

      ticketLock->proc[ticketLock->turn]->state = RUNNABLE;
      cprintf("\n awaked ticket %d \n",ticketLock->proc[ticketLock->turn]->pid);
      return 1;
    // }
  return 0;
  // kfree(first);
  
}
void
initTicket(){
  initTicketLock(&ptable.ticketLock);
}
int
ticketLock(){
  acquireTicketLock(&ptable.ticketLock);
  int ticket = ptable.ticketLock.ticket;
  yield();
    cprintf("\n");
  int i=0;
  int pid = myproc()->pid;
  while (i<2000)
  {
      cprintf("%d",pid);
      i++;
  }
  cprintf("\n");
  releaseTicketLock(&ptable.ticketLock);
  return ticket;
}

int rwinit(){
  rw.sharedData =0;
  rw.writing = 0;
  rw.released=0;
  rw.finished=0;
  for (int i = 0; i < 40; i++)
  {
    rw.job[i] = -1;
  }
  
  initTicketLock(&rw.ticket);
  return 1;
}

void
write(){
int i=0;
while (i<2000)
{
  cprintf("< w_w pid : %d >",myproc()->pid);
  i++;
}

}
void
read(){
  int i=0;
  while (i<1000)
  {
     cprintf(" %d ",myproc()->pid);
      i++;
  }
  
}

int
rwTest(uint pattern){

myproc()->reader = pattern;

cprintf("\n\npid : %d is reader : %d\n",myproc()->pid,pattern);
  if(pattern){
    acquireTicketLock(&rw.ticket);
    yield();
    rw.writing = 1;
    write();
    fetch_and_add(&rw.sharedData,1);
    rw.writing = 0;
    releaseTicketLock(&rw.ticket);
    int i = rw.ticket.turn;
    if(rw.ticket.proc[i]->reader == 0 && i <= rw.ticket.ticket){
      rw.released = 1;
    while(rw.ticket.proc[i+1]->reader == 0 && i+1 <= rw.ticket.ticket){
      releaseTicketLock(&rw.ticket);
      rw.released ++;
      i++;
    }
    
    }
    else{

    }


  }
  else{
    if(rw.writing){
    acquireTicketLock(&rw.ticket);
    yield();
    rw.reading = 1;
    read();
    rw.reading = 0;
    rw.finished++;
    if(rw.finished == rw.released){
      rw.finished = 0;
      rw.released = 0;
      releaseTicketLock(&rw.ticket);
    }
    }
    else{
      if(rw.startReading == 0){
         acquireTicketLock(&rw.ticket);
         yield();
         rw.startReading = 1;
      }

    rw.reading = 1;
    read();
    rw.reading = 0;

    if(rw.ticket.turn <= rw.ticket.ticket-1){
      releaseTicketLock(&rw.ticket);
    }
    }

  }


//cps();














/*
  
  fetch_and_add(&rw.size,1);
  if(pattern){
    if(rw.writing){
    acquireTicketLock(&rw.writer);
    rw.writer.proc[rw.writer.ticket-1]->ticktNumber =  rw.size;
    yield();
    rw.writing = 1;
    write();
    }
    else{
        if(rw.reader.turn >= rw.reader.ticket){
          rw.writing = 1;
          write();
        }
    }

  
  }
  else{
    if(rw.writing){
      acquireTicketLock(&rw.reader);
      rw.reader.proc[rw.reader.ticket-1]->ticktNumber =  rw.size;
      yield();
      rw.writing = 0;
      read();
      releaseTicketLock(&rw.reader);
    }
    else{
      read();
    }
  }
  */
  return rw.sharedData;
}
