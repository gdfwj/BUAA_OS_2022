#include <asm/asm.h>
#include <pmap.h>
#include <env.h>
#include <printf.h>
#include <kclock.h>
#include <trap.h>
typedef struct
{
 int a;
 char b;
 char c;
 int d;
} s1;
typedef struct
{
 int size;
 int c[100];
} s2;
void mips_init()
{
	printf("init.c:\tmips_init() is called\n");
    s1 s0 = {1, 'a', 'b', -1000};
    printf("%$1T", &s0);
    s2 s = {3, {1, 2, 3}};
    printf("%$2T", &s);
    printf("\n");

	//for your degree,don't delete these.
	//------------|
	#ifdef FTEST
	FTEST();
	#endif

	#ifdef PTEST
	ENV_CREATE(PTEST);
	#endif
	//-----------|
	panic("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
}
