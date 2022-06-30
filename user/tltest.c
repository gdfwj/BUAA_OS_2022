#include "lib.h"

// char e[100] = "child is end\n";
// void *printtid(void *msg)
// {
//     writef("tid is: %d, msg is %s\n", gettid(), (char*)msg);
//     pthread_exit(e);
// }

// void umain()
// {
//     u_int ctid;
//     char *meg = "test thread";
//     void *ret;
//     writef("thread test begin\n");
//     pth_init();
//     writef("pthread init ok\n");
//     pthread_create(&ctid, NULL, printtid, meg);
//     writef("create child %d\n", ctid);
//     pthread_join(ctid, &ret);
//     writef("child back meg: %s", (char *)ret);
// }

sem_t sem1, sem2, sem3, sem4;
char *msg = "child exit\n";
void *count12345() {
    sem_wait(&sem1);
    writef("%d: 2\n", gettid());
    sem_post(&sem2);
    sem_wait(&sem3);
    writef("%d: 4\n", gettid());
    sem_post(&sem4);
    pthread_exit(msg);
}

void umain() {
    pthread_t c_tid;
    pth_init();
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);
    sem_init(&sem4, 0, 0);
    pthread_create(&c_tid, NULL, count12345, NULL);
    writef("%d: 1\n", gettid());
    sem_post(&sem1);
    sem_wait(&sem2);
    writef("%d: 3\n", gettid());
    sem_post(&sem3);
    sem_wait(&sem4);
    writef("%d: 5\n", gettid());
}


