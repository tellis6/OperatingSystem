#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// global variables
#define CHILDREN 12
sem_t father;
pthread_mutex_t mother;
int amount;
int day = 0;

/* the mother munction will loop 
for as many days as the user inputs
performing tasks for the children 
while signaling the father for his tasks */
void *motherFunc()
{   
    // infinite loop will continue till the command line argument (amount) decrements to 0
    for (;;) {
        pthread_mutex_lock(&mother);                                        // aquire lock means mother is awake and can perform tasks
        day++;                                                              // initially day 1, will increment every new day
        printf("\nThis is day %d in the life of Mother Hubbard\n\n", day);  // print the day

        for (int i = 1; i <= CHILDREN; i++) {                               // loop for first task
            printf("Child #%d is woken up by mother.\n", i);                // print first task
            usleep(100);                                                    // suspend the thread for 100 microseconds 
        }
        for (int i = 1; i <= CHILDREN; i++) {                               // loop for second task
            printf("Child #%d is fed breakfast by mother.\n", i);           // print second task
            usleep(100);                                                    // suspend the thread for 100 microseconds 
        }
        for (int i = 1; i <= CHILDREN; i++) {                               // loop for third task
            printf("Child #%d is sent to school by mother.\n", i);          // print third task
            usleep(100);                                                    // suspend the thread for 100 microseconds 
        }
        for (int i = 1; i <= CHILDREN; i++) {                               // loop for fourth task
            printf("Child #%d is given dinner by mother.\n", i);            // print fourth task
            usleep(100);                                                    // suspend the thread for 100 microseconds 
        }
        for (int i = 1; i <= CHILDREN; i++) {                               // loop for fifth task
            printf("Child #%d is given a bath by mother.\n", i);            // print fifth task
            sem_post(&father);                                              // signal the father to perform his tasks
            usleep(100);                                                    // suspend the thread for 100 microseconds 
        }
        if (amount == 0) {                                                  // if the command argument, amount, decrements to 0, exit
            break;
        }
    }
}

/* the father munction will loop 
for as many days as the user inputs
performing tasks for the children 
while signaling the mother for her tasks */
void *fatherFunc()
{   
    // infinite loop will continue till the command line argument (amount) decrements to 0
    for (;;) {
        for (int i = 1; i <= CHILDREN; i++) {                       // loop for sixth task
            sem_wait(&father);                                      // wait until mother signals father
            printf("Child #%d is read a book by father.\n", i);     // print fifth task
        }
        for (int i = 1; i <= CHILDREN; i++) {                       // loop for seventh task
            printf("Child #%d is tucked in bed by father.\n", i);   // print seventh task
        }
        amount--;                                                   // decrement the command line argument, amount
        if (amount == 0) {                                          // if the command argument, amount, decrements to 0, exit
            break;
        }
        pthread_mutex_unlock(&mother);                              // release the lock for mother and print the statement
        printf("Father is going to sleep and waking up Mother to take care of the children.\n\n");
    }
}

int main(int argc, char *argv[])
{   
    //Exit program if invalid number of arguments are given
    if (argc != 2) {
        printf("Program requires 1 argument: number of day/night cycles.\n");
        exit(1);
    }
    
    pthread_t m, f;                                         // declare threads
    pthread_mutex_init(&mother, NULL);                      // initialize mutex for mother
    pthread_mutex_unlock(&mother);                          // release lock for mother
    sem_init(&father,0,0);                                  // initialize father semaphore, set to 0
    amount = atoi(argv[1]);                                 // get command line argument for amount of cycles

    pthread_create(&m, NULL, (void *)motherFunc, NULL);     // create mother thread and call mother function
    pthread_create(&f, NULL, (void *)fatherFunc, NULL);     // create father thread and call father function
    pthread_join(m, NULL);                                  // join mother thread
    pthread_join(f, NULL);                                  // join father thread

    pthread_mutex_destroy(&mother);                         // destroy the mother mutex
    sem_destroy(&father);                                   // destroy father semaphore

    return 0;
}