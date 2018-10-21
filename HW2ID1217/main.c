/* 
 * File:   main.c
 * Author: norash1995
 *
 * Created on den 14 februari 2017, 10:06
 */
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#define MAXNROFWORDSINFILE 25143  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */
#define MAXSIZEOFAWORD 30


int numWorkers;           /* number of workers */ 
int maxNrOfWords;         /* the number of words the file contains*/

double start_time, end_time; /* start and end times */
int wordDistSize;  /* nr of words each thread will be appointed*/
char arr[MAXNROFWORDSINFILE][MAXSIZEOFAWORD]; /* array containg the words from the file*/
int palinSums[MAXWORKERS]; /* partial sums */


char word[30];
char reverseW[30];
int len;
int ex;
FILE *palindromesF = fopen("palindromes","w");





/*Since all threads update a shared structure, we
need a mutex for mutual exclusion. The main thread needs to wait for
all threads to complete, it waits for each one of the threads. We specify
a thread attribute value that allow the main thread to join with the
threads it creates. Note also that we free up handles  when they are
no longer needed.
*/
/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  int finalPalinSum;

  FILE *file = fopen("words", "r");


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


  
  /* do the parallel work: create the workers */
  start_time = omp_get_wtime();
  
	omp_set_num_threads(numWorkers);
        
        
        
    #pragma omp parallel for reduction (+: finalPalinSum) private(maxNrOfWords,len,word,reverseW,ex){    
        for (i = 1; i<maxNrOfWords; i++)
    	{
            strcpy(word, arr[i]);
            len = strlen(word);

            int a= len-1;

            //reversing each word
            for(j =0;0<=a; j++,a--){
                reverseW[j] = word[a];
            }
        
            for(j = 0; j<=maxNrOfWords;j++){
                if(strcasecmp(reverseW,arr[j])==0){
                    finalPalinSum= ex++;
                
                    //palinSums[myid]= ex;
                    fprintf(palindromesF,"%s \n",word);
                }
            }
        
            memset(word,0, sizeof word);
            memset(reverseW, 0, sizeof reverseW);
        
        }
    }
  end_time = omp_get_wtime();
  printf("finalPAlinSum: %d\n", finalPalinSum);
  printf("The execution time is %g sec\n", end_time - start_time);
    printf("Main: program completed. Exiting.\n");

    
    
    
#ifdef DEBUG
    printf("Thread[%ld] start-end (%d,%d)\n",myid,start,end);
#endif
    
}