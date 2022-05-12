#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
void sched_yield(void){
	static int count = 0; // remaining time slices of current env
	static int point = 0; // current env_sched_list index
	static int nsch = 0;
    static struct Env *e;
	int flag=0;
	printf("\n");
    if (count == 0 || (e->env_status != ENV_RUNNABLE)) {
		if(e!=NULL) {
			if(e->env_pri%2==0){
				point=nsch+2;
			}
			else point=nsch+1;
			if(point==3) point=0;
			if(point==4) point=1;
			LIST_INSERT_TAIL(&env_sched_list[point], e, env_sched_link);
			//printf("insert in %d\n",point);
		}
		if(!LIST_EMPTY(&env_sched_list[0])){
			//printf("finding list 0\n");
		LIST_FOREACH(e, &env_sched_list[0], env_sched_link) {
			if(e->env_status==ENV_RUNNABLE) {
				flag=1;
				nsch=0;
				count=e->env_pri;
				LIST_REMOVE(e, env_sched_link);
				break;
			}
		}
		}
		if(flag==0 && !LIST_EMPTY(&env_sched_list[1])){
		LIST_FOREACH(e, &env_sched_list[1], env_sched_link) {
			if(e->env_status==ENV_RUNNABLE) {
				flag=1;
				nsch=1;
				count=(e->env_pri)<<1;
				LIST_REMOVE(e, env_sched_link);
				break;
			}
		}
		}
		if(flag==0 && !LIST_EMPTY(&env_sched_list[2])){
		LIST_FOREACH(e, &env_sched_list[2], env_sched_link) {
			if(e->env_status==ENV_RUNNABLE) {
				flag=1;
				nsch=2;
				count=(e->env_pri)<<2;
				LIST_REMOVE(e, env_sched_link);
				break;
			}
		}
		}
		if(flag==0)panic("no runnable env\n");
	}
	count--;
    env_run(e);
}

