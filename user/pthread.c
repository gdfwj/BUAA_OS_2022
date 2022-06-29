#include "lib.h"
#include <mmu.h>
#include <env.h>

#define PTH_FREE 0
#define PTH_RUNNABLE 1
#define PTH_NOT_RUNNABLE 2
#define PTH_WAIT_ONE_END 3
#define pthread_t u_int

struct Pth pths[1024];			  // All pths
extern struct Pth *curpth = NULL; // the current pth

static u_int asid_bitmap[2] = {0};

int pth_alloc(struct Pth **new)
{
	struct Pth *e;
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
		if (now = at)
			return -1;
	}
	pths[now].pth_status = PTH_RUNNABLE;
	*new = &pths[now];
	now++;
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
	}
	int r = pth_alloc(&p); // alloc main thread
	p->if (r < 0)
	{
		user_panic("can not alloc pth");
	}
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
	p->pth_tf.pc = start_routine;
	u_int stack = alloc_stack()
					  p->pth_tf.regs[29] = stack;
	p->pth_tf.regs[4] = arg;
}

void pthread_yield()
{
	static int now = 0;
	int at = now;
	while (1)
	{
		if (pths[now].pth_status == PTH_RUNNABLE)
		{
			break;
		}
		now++;
		if (now == 1024)
			now = 0;
		if (now == at)
		{
			user_panic("no runnable thread\n");
		}
	}
	now++;
	// writef("begin syscall\n");
	if (curpth) // store trapframe and stack
	{
		syscall_get_trapframe(&(curpth->pth_tf));
	}
	writef("syscall_get ok\n");
	curpth->pth_tf.pc += 28;
	curpth = &pths[now];
	syscall_set_trapframe(&(curpth->pth_tf)); // return to new thread
	user_panic("pthread_yield reach end\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
	writef("back ok\n");
}

void pthread_exit(void *retval)
{
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
	curpth = NULL;
	pthread_yield();
}

void pthread_join(u_int thread, void **retval)
{
	curpth->pth_status = PTH_WAIT_ONE_END;
	curpth->pth_waiting = thread;
	writef("begin yield\n");
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