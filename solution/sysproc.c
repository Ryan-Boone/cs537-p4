#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "pstat.h"

int
sys_fork(void)
{
  return fork();
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
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
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

int
sys_settickets(void) // may need to modify global_tickets,global_stride, and global_pass values here
{
  int n;
  if(argint(0, &n) < 0)
    return -1;

  if(n < 1) { //smaller than min
    n = 8;
  } else if (n > (1<<5)) { //greater than max
    n = (1<<5);
  }

  struct proc *p = myproc();
  int old_stride = p->stride;

  p->tickets = n;
  p->stride = (1<<10) / p->tickets;
  p->remain = p->remain * (p->stride / old_stride);
  p->pass = global_pass + p->remain;

  return 0;
}

int
sys_getpinfo(void)
{
  struct pstat *ps;
  
  if(argptr(0, (void*)&ps, sizeof(*ps)) < 0)
    return -1;
    
  return getptableinfo(ps);
}
