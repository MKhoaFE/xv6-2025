extern pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);
#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{

  uint64 va,ua;
  int pnum;

  if(argaddr(0,&va)<0||
    argint(1,&pnum)<0||
    argaddr(2,&ua)){
      return -1;
  }

  if(pnum>PGSIZE*8){
      return -1;
  }
  char* buf = kalloc();
  memset(buf,0,PGSIZE);
  int cnt = (pnum/8+((pnum%8)!=0))*8-pnum;
  printf("cnt = %d\n",cnt);
  for (int i=0;i<pnum;i++,cnt++){
      pte_t* p = walk(myproc()->pagetable,va+i*PGSIZE,0);
      if(*p&PTE_A){
          buf[cnt/8] |= 1<<(cnt%8);
          *p &= ~PTE_A;
      }
  }
  copyout(myproc()->pagetable,ua,buf,pnum);
  kfree(buf);
  return 0;
}
#endif
