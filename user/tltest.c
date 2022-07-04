#include "lib.h"

/*
char e[100] = "child is end\n";
void *printtid(void *msg)
{
    writef("tid is: %d, msg is %s\n", gettid(), (char *)msg);
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
*/

/*
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
    int r;
    pthread_t c_tid;
    pth_init();
    r = sem_init(&sem1, 0, 0);
    if(r<0) user_panic("init sem1 failed\n");
    r = sem_init(&sem2, 0, 0);
    if(r<0) user_panic("init sem2 failed\n");
    sem_init(&sem3, 0, 0);
    sem_init(&sem4, 0, 0);
    pthread_create(&c_tid, NULL, count12345, NULL);
    writef("%d: 1\n", gettid());
    sem_post(&sem1);
    r = sem_wait(&sem2);
    if(r<0) user_panic("P failed\n");
    writef("%d: 3\n", gettid());
    sem_post(&sem3);
    sem_wait(&sem4);
    writef("%d: 5\n", gettid());
}
*/

/*
sem_t sem1, sem2, sem3, sem4, sem5;
char *msg1 = "child 1 exit\n";
char *msg2 = "child 2 exit\n";

void *count1and3(void *msg) {
    writef("tid is: %d, msg is %s\n", gettid(), (char*)msg);
    while(1){
        sem_wait(&sem1);
        writef("thread %d count 1\n", gettid());
        sem_post(&sem2);
        sem_wait(&sem3);
        writef("thread %d count 3\n", gettid());
        sem_post(&sem4);
    }
    pthread_exit(msg1);
}

void count2and4(void *msg)
{
    writef("tid is: %d, msg is %s\n", gettid(), (char*)msg);
    while(1) {
        sem_wait(&sem2);
        writef("thread %d count 2\n", gettid());
        sem_post(&sem3);
        sem_wait(&sem4);
        writef("thread %d count 4\n", gettid());
        sem_post(&sem5);
    }
    pthread_exit(msg2);
}

void umain() {
    pthread_t c_tid;
    char *cmeg1 = "test thread 1";
    char *cmeg2 = "test thread 2";
    int r;
    pth_init();
    r = sem_init(&sem1, 0, 0);
    if(r<0) user_panic("init sem1 failed\n");
    r = sem_init(&sem2, 0, 0);
    if(r<0) user_panic("init sem2 failed\n");
    r = sem_init(&sem3, 0, 0);
    if(r<0) user_panic("init sem3 failed\n");
    r = sem_init(&sem4, 0, 0);
    if(r<0) user_panic("init sem4 failed\n");
    r = sem_init(&sem5, 0, 0);
    if(r<0) user_panic("init sem5 failed\n");
    pthread_create(&c_tid, NULL, count1and3, cmeg1);
    writef("child %d create\n", c_tid);
    pthread_create(&c_tid, NULL, count2and4, cmeg2);
    writef("child %d create\n", c_tid);
    while(1){
        writef("thread %d count 0\n", gettid());
        sem_post(&sem1);
        sem_wait(&sem5);
        writef("thread %d count 5\n", gettid());
        writef("finish one count\n");
    }
}
*/

sem_t sem1, sem2;
int count;
void chi(void *msg)
{
    writef("tid is: %d, msg is %s\n", gettid(), (char *)msg);
    while (1)
    {
        sem_wait(&sem2);
        writef("thread %d said 1 and count %d\n", gettid(), count);
        sem_post(&sem1);
    }
}
void chii(void *msg)
{
    writef("tid is: %d, msg is %s\n", gettid(), (char *)msg);
    while (1)
    {
        sem_wait(&sem1);
        writef("thread %d said 2 and count %d\n", gettid(), count);
        sem_post(&sem2);
    }
}
void umain()
{
    pthread_t c_tid;
    void *ret;
    char *cmeg1 = "test thread begin";
    pth_init();
    sem_init(&sem1, 0, 0);
    sem_init(&sem2, 0, 1);
    int i;
    for (i = 0; i < 1023; i++)
    {
        pthread_create(&c_tid, NULL, chi, cmeg1);
    }
    pthread_join(c_tid, &ret);
}