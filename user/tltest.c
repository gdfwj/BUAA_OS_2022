#include "lib.h"

char e[100] = "child is end\n";
void *printtid(char *msg)
{
    writef("tis is: %d, msg is %s\n", gettid(), msg);
    pthread_exit(e);
}

void umain()
{
    u_int ctid;
    char *meg = "test thread";
    void *ret;
    writef("thread test begin\n");
    pth_init();
    writef("pthread init ok\n");
    pthread_create(&ctid, NULL, printtid, meg);
    writef("create child %d\n", ctid);
    pthread_join(ctid, &ret);
    writef("child back meg: %s", (char *)ret);
}
