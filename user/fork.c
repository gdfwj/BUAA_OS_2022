// implement fork from user space

#include "lib.h"
#include <mmu.h>
#include <env.h>


/* ----------------- help functions ---------------- */

/* Overview:
 * 	Copy `len` bytes from `src` to `dst`.
 *
 * Pre-Condition:
 * 	`src` and `dst` can't be NULL. Also, the `src` area
 * 	 shouldn't overlap the `dest`, otherwise the behavior of this
 * 	 function is undefined.
 */
void user_bcopy(const void *src, void *dst, size_t len)
{
	void *max;

	//	writef("~~~~~~~~~~~~~~~~ src:%x dst:%x len:%x\n",(int)src,(int)dst,len);
	max = dst + len;

	// copy machine words while possible
	if (((int)src % 4 == 0) && ((int)dst % 4 == 0)) {
		while (dst + 3 < max) {
			*(int *)dst = *(int *)src;
			dst += 4;
			src += 4;
		}
	}

	// finish remaining 0-3 bytes
	while (dst < max) {
		*(char *)dst = *(char *)src;
		dst += 1;
		src += 1;
	}

	//for(;;);
}

/* Overview:
 * 	Sets the first n bytes of the block of memory
 * pointed by `v` to zero.
 *
 * Pre-Condition:
 * 	`v` must be valid.
 *
 * Post-Condition:
 * 	the content of the space(from `v` to `v`+ n)
 * will be set to zero.
 */
void user_bzero(void *v, u_int n)
{
	char *p;
	int m;

	p = v;
	m = n;

	while (--m >= 0) {
		*p++ = 0;
	}
}
/*--------------------------------------------------------------*/

/* Overview:
 * 	Custom page fault handler - if faulting page is copy-on-write,
 * map in our own private writable copy.
 *
 * Pre-Condition:
 * 	`va` is the address which leads to a TLBS exception.
 *
 * Post-Condition:
 *  Launch a user_panic if `va` is not a copy-on-write page.
 * Otherwise, this handler should map a private writable copy of
 * the faulting page at correct address.
 */
/*** exercise 4.13 ***/
static void
pgfault(u_int va)
{
	u_int tmp;
	int ret;
	va = ROUNDDOWN(va, BY2PG);
	//	writef("fork.c:pgfault():\t va:%x\n",va);
	if ((((Pte*)(*vpt))[VPN(va)]&PTE_COW) == 0) {
        user_panic("User pgfault haddler facing a non-COW page\n");
    }
	//map the new page at a temporary place
	tmp = USTACKTOP;
    ret = syscall_mem_alloc(0, tmp, ((Pte*)(*vpt))[VPN(va)]&(~PTE_COW)&0xfff);
    if (ret<0) {
        user_panic("User pgfault haddler mem_alloc faild\n");
    }
	//copy the content
	user_bcopy((void*)ROUNDDOWN(va,BY2PG), (void*)tmp, BY2PG);
	//map the page on the appropriate place
	ret = syscall_mem_map(0, tmp, 0, va, ((Pte*)(*vpt))[VPN(va)]&(~PTE_COW)&0xfff);
	if (ret<0) {
        user_panic("User pgfault haddler mem_map faild\n");
    }
	//unmap the temporary place
	ret = syscall_mem_unmap(0, tmp);
    if (ret<0) {
        user_panic("User pgfault haddler mem_unmap failed\n");
    }
}

/* Overview:
 * 	Map our virtual page `pn` (address pn*BY2PG) into the target `envid`
 * at the same virtual address.
 *
 * Post-Condition:
 *  if the page is writable or copy-on-write, the new mapping must be
 * created copy on write and then our mapping must be marked
 * copy on write as well. In another word, both of the new mapping and
 * our mapping should be copy-on-write if the page is writable or
 * copy-on-write.
 *
 * Hint:
 * 	PTE_LIBRARY indicates that the page is shared between processes.
 * A page with PTE_LIBRARY may have PTE_R at the same time. You
 * should process it correctly.
 */
/*** exercise 4.10 ***/
static void
duppage(u_int envid, u_int pn)
{
	u_int addr;
	u_int perm;
	int flag=0;
	addr = pn <<PGSHIFT;
	perm = (*vpt)[pn] & 0xfff;
	if((perm & PTE_R) && !(perm & PTE_LIBRARY)) {
		perm = PTE_COW | perm;
		flag=1;
	}
	syscall_mem_map(0, addr, envid, addr, perm);
	if(flag==1) syscall_mem_map(0, addr, 0, addr, perm);
	//writef("dumppage return");

	//	user_panic("duppage not implemented");
}
int make_shared(void *va) {
	int ret;
	//va = ROUND(va, BY2PG);
	u_int pn = VPN(va);
	u_int addr = pn<<PGSHIFT;
	u_int envid = syscall_getenvid();
	if(addr>=UTOP) return -1;
	if(!((((Pde*)(*vpd))[addr>>PDSHIFT]&PTE_V) &&
             (((Pte*)(*vpt))[addr>>PGSHIFT]&PTE_V))) {
		ret = syscall_mem_alloc(envid, addr, PTE_V | PTE_R);
		if(ret<0) {
			return -1;
		}
	}
	u_int perm = (*vpt)[pn] & 0xfff;
	if(perm & PTE_R ==0) {
		return -1;
	}
	perm = perm | PTE_LIBRARY;
	syscall_mem_map(0, addr, envid, addr, perm);
	return ROUND(((Pte*)(*vpt))[addr>>PGSHIFT],BY2PG);
}
/* Overview:
 * 	User-level fork. Create a child and then copy our address space
 * and page fault handler setup to the child.
 *
 * Hint: use vpd, vpt, and duppage.
 * Hint: remember to fix "env" in the child process!
 * Note: `set_pgfault_handler`(user/pgfault.c) is different from
 *       `syscall_set_pgfault_handler`.
 */
/*** exercise 4.9 4.15***/
extern void __asm_pgfault_handler(void);
int
fork(void)
{
	// Your code here.
	u_int newenvid;
	extern struct Env *envs;
	extern struct Env *env;
	u_int i;
	int j,k,ret;
	//Pde* pgdir_entry=(Pde*)vpd;
	//Pte* pgtable_entry=(Pte*)vpt;
	
	//The parent installs pgfault using set_pgfault_handler
	set_pgfault_handler(pgfault);
	//writef("fork begin\n");
	//writef("call syscall_env_alloc\n");
	newenvid = syscall_env_alloc();
	//writef("syscall return\n");
	if(newenvid==0) {
		env = &envs[ENVX(syscall_getenvid())];
		return 0;
	}
	//for(j=0;j<1024;j++) {
	//	if((*(pgdir_entry+j))&PTE_V){
	//		pgtable_entry = KADDR(*(pgdir_entry+i));
	//		for(k=0;k<1024;k++) {
	//			if((*(pgtable_entry+k))&PTE_V) {
	//				i=KADDR(*(pgtable_entry+k));
	//				duppage(newenvid, VPN(i));
	//			}
	//		}
	//	}
	//}
	//writef("begin dumppage\n");
	for (i=0; i < USTACKTOP; i+=BY2PG) {
		//if(i>=0x7f3fd000)writef("i is %x\n",i);
        if ((((Pde*)(*vpd))[i>>PDSHIFT]&PTE_V) &&
            (((Pte*)(*vpt))[i>>PGSHIFT]&PTE_V)) {
				//writef("i:%x VPN:%x\n", i, VPN(i));
				//writef("call dumppage\n");
                duppage(newenvid, VPN(i));
				//writef("dumppage return\n");
            }
    }
	//writef("dumppage end");
	syscall_mem_alloc(newenvid, UXSTACKTOP-BY2PG, PTE_V | PTE_R);
    syscall_set_pgfault_handler(newenvid, __asm_pgfault_handler, UXSTACKTOP);
    syscall_set_env_status(newenvid, ENV_RUNNABLE);
	//alloc a new alloc
	return newenvid;
}

// Challenge!
int
sfork(void)
{
	user_panic("sfork not implemented");
	return -E_INVAL;
}
