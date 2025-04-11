#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

extern uint64 freebytes(void);
extern uint64 procnum(void);


uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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

uint64
sys_trace(void) {
  argint(0, &(myproc()->trace_mask));
  return 0;
}

uint64
sys_sysinfo(void) {
  struct sysinfo _sysinfo;
  _sysinfo.freemem = freebytes();
  _sysinfo.nproc = procnum();

  uint64 destaddr;
  argaddr(0, &destaddr);

  if (copyout(myproc()->pagetable, destaddr, (char*) &_sysinfo, sizeof _sysinfo) < 0) {
    return -1;
  }

  return 0;
}

uint64
sys_pgaccess(void)
{
    uint64 start_va; // Địa chỉ bắt đầu
    int num_pages;   // Số lượng pages
    uint64 user_mask; // Địa chỉ user-space để lưu kết quả

    if(argaddr(0, &start_va) < 0 || argint(1, &num_pages) < 0 || argaddr(2, &user_mask) < 0)
        return -1;

    if(num_pages <= 0 || num_pages > 64)
        return -1;

    uint64 mask = 0;
    pagetable_t pagetable = myproc()->pagetable;

    for(int i = 0; i < num_pages; i++){
        uint64 va = start_va + i * PGSIZE;
        pte_t *pte = walk(pagetable, va, 0);
        if(pte == 0)
            continue;

        if(*pte & PTE_A){
            mask |= (1L << i);     // Đánh dấu bit thứ i
            *pte &= ~PTE_A;        // Clear PTE_A sau khi check
        }
    }

    if(copyout(pagetable, user_mask, (char *)&mask, sizeof(mask)) < 0)
        return -1;

    return 0;
}
