#include "lib.h"
#include <mmu.h>
#include <env.h>

#define PTH_FREE 0
#define PTH_RUNNABLE 1
#define PTH_NOT_RUNNABLE 2
#define PTH_WAIT_ONE_END 3
#define pthread_t u_int

struct Pth pths[1024];			  // All pths
struct Pth *curpth = NULL; // the current pth

static u_int asid_bitmap[2] = {0};
struct Sem sems[1024];

int pth_alloc(struct Pth **new)
{
	static int now = 0;
	int at = now;
	while (1)
	{
		if (pths[now].pth_status == PTH_FREE)
		{
			break;
		}
		now++;
		if (now == 1024)
			now = 0;
		if (now == at)
			user_panic("no more pths\n");
	}
	pths[now].pth_status = PTH_RUNNABLE;
	*new = &pths[now];
	now++;
	return 0;
}

u_int alloc_stack()
{
	static u_int p_stack = USTACKTOP;
	p_stack -= BY2PG;
	return p_stack;
}

void pth_init()
{
	struct Pth *p;
	int i;
	// LIST_INIT(&pth_sched_list);
	for (i = 0; i < 1024; i++)
	{ // initialize pths
		pths[i].pth_id = i;
		pths[i].pth_status = PTH_FREE;
		sems[i].pointer=NULL;
	}
	int r = pth_alloc(&p); // alloc main thread
	if (r < 0)
	{
		user_panic("can not alloc pth");
	}
	p->pth_tf.cp0_status = 0x1000100c;
	curpth = p;
}

int pthread_create(pthread_t *id, const void *attr, void *(*start_routine)(void *), void *arg)
{
	struct Pth *p;
	int r;
	r = pth_alloc(&p);
	writef("alloc ok\n");
	if (r < 0)
	{
		user_panic("can not alloc pth\n");
	}
	*id = p->pth_id;
	user_bzero((void *)&p->pth_tf, sizeof(struct Trapframe));
	//writef("pc: %x", start_routine);
	p->pth_tf.cp0_epc = start_routine;
	u_int stack = alloc_stack();
	p->pth_tf.regs[29] = stack;
	p->pth_tf.regs[4] = arg;
	p->pth_tf.cp0_status = 0x1000100c;
}

void pthread_yield()
{
	//static int count[1024]={0};
	static int nowt = 0;
	int at = nowt;
	//writef("at: %d\n", at);
	while (1)
	{
		//writef("nowt: %d, status: %d\n", nowt, pths[nowt].pth_status);
		if (pths[nowt].pth_status == PTH_RUNNABLE)
		{
			break;
		}
		nowt++;
		if (nowt == 1024)
			nowt = 0;
		if (nowt == at)
		{
			user_panic("no runnable thread\n");
		}
	}
	//writef("begin syscall\n");
	//writef("stack place: %x\n", &(curpth->pth_tf));
	if (curpth) // store trapframe and stack
	{
		syscall_get_trapframe(&(curpth->pth_tf));
		//curpth->pth_tf.pc += 12;
	}
	//count[curpth->pth_id]++;
	//writef("tid: %d, count: %d\n", curpth->pth_id, count[curpth->pth_id]);
	//if(count[curpth->pth_id]%2==1) {
		curpth = &pths[nowt];
		//writef("pthid: %x, pc: %x\n", curpth->pth_id, curpth->pth_tf.pc);
		nowt++;
		syscall_set_trapframe(&(curpth->pth_tf)); // return to new thread
		//user_panic("pthread_yield reach end, tid: %d\n", curpth->pth_id);
	//}
	//else 
	writef("back ok\n");
}

void pthread_exit(void *retval)
{
	//writef("exit begin\n");
	curpth->pth_status = PTH_FREE;
	int i;
	for (i = 0; i < 1024; i++)
	{
		if (pths[i].pth_status == PTH_WAIT_ONE_END && pths[i].pth_waiting == curpth->pth_id)
		{
			pths[i].pth_status = PTH_RUNNABLE;
			pths[i].pth_waiting_data = retval;
		}
	}
	//writef("exit to yield\n");
	pthread_yield();
}

void pthread_join(u_int thread, void **retval)
{
	curpth->pth_status = PTH_WAIT_ONE_END;
	curpth->pth_waiting = thread;
	//writef("begin yield\n");
	pthread_yield();
	*retval = curpth->pth_waiting_data;
}

int pthread_cancel(pthread_t thread)
{
	if (pths[thread].pth_status != PTH_FREE)
	{
		pths[thread].pth_status = PTH_FREE;
		return 0;
	}
	return -1;
}

int gettid()
{
	return curpth->pth_id;
}

int sem_init(sem_t *sem, int pshared, unsigned int value) {

	int sem_num;
	for(sem_num=0;sem_num<1024;sem_num++) {
		if(sems[sem_num].pointer==NULL) break;
	}
	if(sem_num==1024) return -1;
	if(value<0) {
		user_panic("error! invalid value\n");
	}
	*sem = value;
	sems[sem_num].pointer = sem;
	sems[sem_num].share = pshared ? 1: 0;
	sems[sem_num].envid = syscall_getenvid();
	return 0;
}

int sem_destroy(sem_t *sem) {
	int i;
	for(i=0;i<1024;i++) {
		if(sems[i].pointer==sem) break;
	}
	if(i==1024) return -1;
	if(sems[i].share==0 && sems[i].envid!=syscall_getenvid()) {
		user_panic("no permission to destory\n");
	}
	sems[i].pointer=NULL;
	return 0;
}

int sem_wait(sem_t *sem) {
	int i;
	//writef("in wait\n");
	for(i=0;i<1024;i++) {
		if(sems[i].pointer==sem) break;
	}
	if(i==1024) return -1;
	if(sems[i].share==0 && sems[i].envid!=syscall_getenvid()) {
		user_panic("no permission to P\n");
	}
	if((*sem )> 0) {
		*sem=*sem-1;
	}
	else {
		sems[i].queue[sems[i].tail]=gettid();
		sems[i].tail++;
		while(*sem==0) {
			curpth->pth_status=PTH_NOT_RUNNABLE;
			pthread_yield();
		}
		*sem-=1;
	}
	return 0;
}

int sem_trywait(sem_t *sem) {
	int i;
	for(i=0;i<1024;i++) {
		if(sems[i].pointer==sem) break;
	}
	if(i==1024) return -1;
	if(sems[i].share==0 && sems[i].envid!=syscall_getenvid()) {
		user_panic("no permission to try_wait\n");
	}
	if((*sem )> 0) {
		*sem=*sem-1;
	}
	else {
		while(*sem==0) {
			pthread_yield();
		}
		*sem-=1;
	}
	return 0;
}

int sem_post(sem_t *sem) {
	int i;
	for(i=0;i<1024;i++) {
		if(sems[i].pointer==sem) break;
	}
	if(i==1024) return -1;
	writef("%d: post sem%d\n", gettid(), i+1);
	if(sems[i].share==0 && sems[i].envid!=syscall_getenvid()) {
		user_panic("no permission to V\n");
	}
	*sem+=1;
	if(*sem==1 && sems[i].head!=sems[i].tail) {
		writef("wake %d\n", pths[sems[i].queue[sems[i].head]].pth_id);
		pths[sems[i].queue[sems[i].head]].pth_status = PTH_RUNNABLE;
		sems[i].head++;
		*sem-=1;
	}
	return 0;
}

int sem_getvalue(sem_t *sem, int *sval) {
	int i;
	for(i=0;i<1024;i++) {
		if(sems[i].pointer==sem) break;
	}
	if(i==1024) return -1;
	if(sems[i].share==0 && sems[i].envid!=syscall_getenvid()) {
		user_panic("no permission to get\n");
	}
	*sval = *sem;
	return 0;
}

