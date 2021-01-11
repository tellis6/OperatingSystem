#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// global variables
#define MAX_SIZE 5 
pthread_mutex_t bufLock;
sem_t empty, full;
int value;

/*producer function, 
to produce a printable character into the buffer 
if there is available space*/
void *producer(char* buf)
{   
    // this will be used to represent index
    int in = 0;

    // infinite loop to continue producing until user exits
    for(;;) {
        sem_wait(&full);                                            // wait till there is available space
        pthread_mutex_lock(&bufLock);                               // aquire lock to access critical section
        char pItem = 'a' + (rand() % 26);                           // generate a random character to place in buffer
        buf[in] = pItem;                                            // place the character in the buffer
        printf("Producer inserted %c at index %d\n", buf[in],in);   // print a message showing the new character and its buffer location
        in = (in+1) % MAX_SIZE;                                     // increment index till MAX_SIZE, then reset to 0
        pthread_mutex_unlock(&bufLock);                             // release lock
        sem_post(&empty);                                           // signal the consumer
    }
}

void *consumer(char* buf)
{   
    // this will be used to represent index
    int out = 0;

    // infinite loop to continue consuming until user exits
    for(;;) {
        sem_wait(&empty);                                           // wait till there is a character to consume
        pthread_mutex_lock(&bufLock);                               // aquire lock to access critical section
        char cItem = buf[out];                                      // variable to hold the character from the 'out' position in buffer
        printf("Consumer removed %c from index %d\n",cItem, out);   // print a message showing the character to be removed and its buffer location
        out = (out+1) % MAX_SIZE;                                   // increment index till MAX_SIZE, then reset to 0
        pthread_mutex_unlock(&bufLock);                             // release lock
        sem_post(&full);                                            // signal the producer
    }
}

int main()
{   
    srand(time(NULL));                                      // used to generate random char
    char buffer[MAX_SIZE];                                  // create the buffer with size MAX_SIZE
    pthread_t p, c;                                         // declare threads
    pthread_mutex_init(&bufLock, NULL);                     // initialize mutex 
    sem_init(&full,0,MAX_SIZE);                             // initialize 'full' semaphore, set to MAX_SIZE 
    sem_init(&empty,0,0);                                   // initialize 'empty' semaphore, set to 0

    pthread_create(&p, NULL, (void *)producer, &buffer);    // create producer thread and call the producer function, passing buffer
    pthread_create(&c, NULL, (void *)consumer, &buffer);    // create producer thread and call the consumer function, passing buffer
    
    pthread_join(p, NULL);                                  // join the producer thread
    pthread_join(c, NULL);                                  // join the consumer thread

    pthread_mutex_destroy(&bufLock);                        // destroy mutex
    sem_destroy(&full);                                     // destroy 'full' semaphore
    sem_destroy(&empty);                                    // destroy 'empty' semaphore

    return 0;
}
