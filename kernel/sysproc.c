#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 upage_addr;
  int page_nums;
  uint64 ubitmap_addr;

  argaddr(0, &upage_addr);
  argint(1, &page_nums);
  argaddr(2, &ubitmap_addr);

  if (upage_addr >= MAXVA || (upage_addr+PGSIZE >= MAXVA)) {
    return -1;
  }

  uint64 bitmap = 0;
  // uint64 start_addr = PGROUNDDOWN(upage_addr);
  struct proc *pr = myproc();
  if (page_nums > 64)
  {
    return -1;
  }
  
  for(int i = 0; i < page_nums; i++){
    uint64 va = upage_addr + i*PGSIZE;
    pte_t *pte = walk(pr->pagetable, va, 0);
    if (pte != 0 && (*pte & PTE_V) && (*pte & PTE_U) && (*pte & PTE_A)){
      bitmap |= 1 << i;
      *pte = (*pte) & (~PTE_A);
    }else{
      bitmap |= 0 << i;
    }
  }
  copyout(pr->pagetable, ubitmap_addr, (char*)&bitmap, sizeof(bitmap));
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}