/* 
 * File:   main.c
 * Author: norash1995
 *
 * Created on den 17 februari 2017, 12:40
 */
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#define SHARED 1
void *Producer(void *); /*the two threads*/
void *Consumer(void *);
sem_t empty, full;  /*the global semaphores*/
int data;   /*shared buffer*/
int numIters;
/*main() -- read command line, create threads, join them and print result*/


/*
 * 
 */
int main(int argc, char** argv) {

    pthread_t pid, cid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    numIters = atoi(argv[1]);
    sem_init(&empty, SHARED, 1);    /* sem empty =1 */
    sem_init(&full, SHARED, 0);     /*sem full = 0*/
    printf("main started\n");
    pthread_create(&pid, &attr, Producer, NULL);
    pthread_create(&cid, &attr, Consumer, NULL);
    pthread_join(pid, NULL);
    pthread_join(cid, NULL);
    printf("main done\n");
     pthread_exit(NULL);
}

/*deposit 1, ..., numIters into the data buffer*/
void *Producer( void *arg){
    int produced;
    printf("Producer created\n");
    for(produced = 0; produced < numIters; produced++){
        sem_wait(&empty);
        data = produced;
        sem_post(&full);
    }
    return NULL;
}

void *Consumer(void *arg){
    int total = 0, consumed;
    printf("Consumer created\n");
    for(consumed = 0; consumed < numIters; consumed++){
        sem_wait(&full);
        total = total +data;
        sem_post(&empty);
    }
    printf("for %d iterations, the total is %d\n", numIters, total);
    return NULL;
}