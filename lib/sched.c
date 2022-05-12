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
	int pp;
	printf("\n");
    if (count == 0 || (e->env_status != ENV_RUNNABLE)) {
		if(e!=NULL) {
			if(e->env_pri%2==0){
				pp=nsch+2;
			}
			else pp=nsch+1;
			if(pp==3) point=0;
			if(pp==4) point=1;
			LIST_INSERT_TAIL(&env_sched_list[point], e, env_sched_link);
			//printf("insert in %d\n",point);
		}
		while(flag==0){
			if(!LIST_EMPTY(&env_sched_list[point])){
				LIST_FOREACH(e, &env_sched_list[point], env_sched_link) {
					if(e->env_status==ENV_RUNNABLE) {
						flag=1;
						nsch=0;
						count=(e->env_pri)<<point;
						LIST_REMOVE(e, env_sched_link);
						break;
					}
				}
			}
			point+=1;
			if(point==3) point=0;
		}
	}
	count--;
    env_run(e);
}

