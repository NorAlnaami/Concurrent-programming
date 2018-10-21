#ifndef _REENTRANT 
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#define MAXNROFWORDSINFILE 25143  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define MAXSIZEOFAWORD 30


pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int maxNrOfWords;         /* the number of words the file contains*/
int numArrived = 0;       /* number who have arrived */

double start_time, end_time; /* start and end times */
int wordDistSize;  /* nr of words each thread will be appointed*/
char arr[MAXNROFWORDSINFILE][MAXSIZEOFAWORD]; /* array containg the words from the file*/
char reverseArr[MAXNROFWORDSINFILE][MAXSIZEOFAWORD]; /*reverse array containing all words*/
int palinSums[MAXWORKERS]; /* partial sums */
int finalPalinSum;
FILE *palindromesF;

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

void *Worker(void *);




/*Since all threads update a shared structure, we
need a mutex for mutual exclusion. The main thread needs to wait for
all threads to complete, it waits for each one of the threads. We specify
a thread attribute value that allow the main thread to join with the
threads it creates. Note also that we free up handles  when they are
no longer needed.
*/
/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j,rc;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];
  FILE *file = fopen("words", "r");
  void *status;

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* read command line args if any */
  maxNrOfWords = (argc > 1)? atoi(argv[1]): MAXNROFWORDSINFILE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  wordDistSize = maxNrOfWords/numWorkers;
  
  //opening the file for reading
  if(fopen("words", "r")==NULL)
  {
      printf("File cannot open");
  }
  
  /*Initialize the stringArray*/
  for(i = 0; i<maxNrOfWords; i++){
      fscanf(file,"%s", arr[i]);
  }
  fclose(file);
  

  /* print the stringArray */
#ifdef DEBUG
  for (i = 0; i < maxNrOfWords; i++) {
      printf("stringArray[%d]: %s\n",i,arr[i]);
  }
#endif

  
    /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  
  /* do the parallel work: create the workers */
  start_time = read_timer();
  
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  
     /* joining the threads in order to find the total nr of palindromes*/

   for(i=0; i<numWorkers; i++) {
       pthread_join(workerid[i], &status);
       //printf("Thread %d join\n",i);
   }
  
  for(i= 0; i<numWorkers; i++){
      printf("Thread [%d] palisums: %d\n",i,palinSums[i]);
      finalPalinSum+= palinSums[i];
  }
  end_time = read_timer();
  printf("finalPAlinSum: %d\n", finalPalinSum);
  printf("The execution time is %g sec\n", end_time - start_time);
    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);
}








void *Worker(void *arg){
    int i,j;
    long myid = (long)arg;
    char word[30];
    char reverseW[30];
    int len;
    int ex;
    palindromesF = fopen("palindromes","w");
    
    //assigning the start for each thread and the end
    int start = myid*wordDistSize +1;
    int end = start + wordDistSize -1;
    
    
    
#ifdef DEBUG
    printf("Thread[%ld] start-end (%d,%d)\n",myid,start,end);
#endif
    
    for (i = start-1; i<end; i++)
    {
        strcpy(word, arr[i]);
        len = strlen(word);

        int a= len-1;
        //printf("a:%d word: %s\n",a, word);
        
        //reversing each word
        for(j =0;0<=a; j++,a--){
            reverseW[j] = word[a];
        }
        //for loop
        //comparing the strings
        if(strcmp(reverseW,word)==0){
            ex++;
            /*The lock statement causes all other threads to wait for our instance to finish*/
            /*
        Lock a mutex prior to put palindromes in the file, and unlock it after updating.
        */
            pthread_mutex_lock(&barrier);
            fprintf(palindromesF,"%s \n",word);
            pthread_mutex_unlock(&barrier);
            
        }
        /*
        Lock a mutex prior to update the value of the shared variable palinSums, and unlock it after updating.
        */
        pthread_mutex_lock(&barrier);
        palinSums[myid]= ex;
        pthread_mutex_unlock(&barrier);
        //printf("length %d reverse:%s word:%s\n",len,reverseW,word);
        //printf("Word: %s Reverse:%s\n",word,reverseW);
        memset(word,0, sizeof word);
        memset(reverseW, 0, sizeof reverseW);
        
    }
}