#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

// global variables
sem_t bSem, sSem, fSem, pSem, waitSem;
pthread_mutex_t pLock, bLock, sLock, fLock;
int pSize, bSize, sSize, fSize;
int bCount = 0, sCount = 0, fCount = 0, pCount = 0;
int* pArr;
int* bArr;
int* sArr;

/* baggage funtion will loop until 
the amount of passengers have all been served 
and will then signal the security function */
void *baggage()
{   
    // loop till all passengers have been served
    while (bCount < pSize) {
        sem_wait(&bSem);                                                            // wait until baggage is signaled
        if (bCount >= pSize) {                                                      // if all passengers served, exit
            break;                                                                  
        }
        pthread_mutex_lock(&bLock);                                                 // aquire baggage lock for critical section
        int pass = pArr[bCount];                                                    // create variable to hold the current passenger from passenger list
        bArr[bCount] = pass;                                                        // set current index of baggage list to be current passenger
        bCount++;                                                                   // increment the baggage index
        pthread_mutex_unlock(&bLock);                                               // release the baggage lock
        usleep(100);                                                                // suspend thread for 100 microseconds
        printf("Passenger %d is waiting to be screened by a screener.\n", pass);    // print passenger number and task
        
        // if the index reaches the passenger amount, signal each baggage semaphore
        if (bCount >= pSize) {                                                      
            for (int i = 0; i < bSize; i++) {
                sem_post(&bSem);
            }
        }
        sem_post(&sSem);                                                            // signal security screeners
    }
}

/* security funtion will loop until 
the amount of passengers have all been served 
and will then signal the flight function */
void *security()
{   
    // loop till all passengers have been served
    while (sCount < pSize) {
        sem_wait(&sSem);                                                                // wait until baggage is signaled
        if (sCount >= pSize) {                                                          // if all passengers served, exit
            break;
        }
        pthread_mutex_lock(&sLock);                                                     // aquire security lock for critical section
        int pass = bArr[sCount];                                                        // variable to hold current passenger from baggage list
        sArr[sCount] = pass;                                                            // set current index of security list to be current passenger
        sCount++;                                                                       // increment the security index
        pthread_mutex_unlock(&sLock);                                                   // release security lock
        usleep(100);                                                                    // suspend thread for 100 microseconds
        printf("Passenger %d is waiting to board the plane by an attendant.\n", pass);  // print passenger number and task

        // if the index reaches the passenger amount, signal each security semaphore
        if (sCount >= pSize) {                                                          
            for (int i = 0; i < sSize; i++) {
                sem_post(&sSem);
            }
        }
        sem_post(&fSem);                                                                // signal flight assistant
    }
}

/* flight funtion will loop until 
the amount of passengers have all been served */
void *flight()
{   
    // loop till all passengers have been served
    while (fCount < pSize) {
        sem_wait(&fSem);                                                // wait until baggage is signaled
        if (fCount >= pSize) {                                          // if all passengers served, exit
            break;
        }
        pthread_mutex_lock(&fLock);                                     // aquire flight lock for critical section
        int pass = sArr[fCount];                                        // variable to hold current passenger from security list
        fCount++;                                                       // increment the flight index
        pthread_mutex_unlock(&fLock);                                   // release flight lock
        usleep(100);                                                    // suspend thread for 100 microseconds
        printf("Passenger %d has been seated and relaxes.\n", pass);    // print passenger number and task

        // if the index reaches the passenger amount, signal each flight semaphore
        if (fCount >= pSize) {
            for (int i = 0; i < fSize; i++) {
                sem_post(&fSem);
            }
        }
    }
}

/* terminal function */
void *terminal(void *passNo)
{   
    int pass = *((int *)passNo);                                                        // convert void* to int, represents current passenger
    printf("Passenger %d arrived at the terminal.\n", pass);                            // print passenger number and task
    sem_wait(&waitSem);                                                                 // wait for next in line
    sem_wait(&pSem);                                                                    // wait for passenger
    pArr[pCount] = pass;                                                                // passenger is added to current index of passenger list
    pCount++;                                                                           // index is incremented
    sem_post(&pSem);                                                                    // signal next passenger
    usleep(100);                                                                        // suspend thread for 100 microseconds
    printf("Passenger %d is waiting at baggage processing for a handler.\n", pass);     // print passenger number and task
    sem_post(&bSem);                                                                    // signal baggage semaphore
}

int main(int argc, char *argv[])
{   
    //Exit program if invalid number of arguments are given
    if (argc != 5) {
        printf("Program requires 4 argument: (1) number of passengers, (2) number of baggage handlers (3) number of security screeners (4) number of flight attendants.\n");
        exit(1);
    }
    
    pSize = atoi(argv[1]);              // set passenger size to second command line argument (first argument is program name)
    bSize = atoi(argv[2]);              // set baggage size to third command line argument
    sSize = atoi(argv[3]);              // set security size to fourth command line argument
    fSize = atoi(argv[4]);              // set flight size to fifth command line argument
    pthread_t p[pSize];                 // declare thread list, size of pSize
    pthread_t b[bSize];                 // declare thread list, size of bSize
    pthread_t s[sSize];                 // declare thread list, size of sSize
    pthread_t f[fSize];                 // declare thread list, size of fSize
    sem_init(&pSem,0,1);                // initialize passenger semaphore, set to 1
    sem_init(&bSem,0,0);                // initialize baggage semaphore, set to 0
    sem_init(&sSem,0,0);                // initialize security semaphore, set to 0
    sem_init(&fSem,0,0);                // initialize flight semaphore, set to 0
    sem_init(&waitSem, 0, 0);           // initialize waiting line semaphore, set to 0
    pthread_mutex_init(&pLock, NULL);   // initialize passenger mutex lock
    pthread_mutex_init(&bLock, NULL);   // initialize baggage mutex lock
    pthread_mutex_init(&sLock, NULL);   // initialize security mutex lock
    pthread_mutex_init(&fLock, NULL);   // initialize flight mutex lock

    pArr = malloc(sizeof(int) * pSize); // allocate pSize amount of memory to global passenger list/string
    memset(pArr, 0, sizeof(int));       // initialize to 0

    bArr = malloc(sizeof(int) * pSize); // allocate pSize amount of memory to global baggage list/string
    memset(bArr, 0, sizeof(int));       // initialize to 0

    sArr = malloc(sizeof(int) * pSize); // allocate pSize amount of memory to global security list/string
    memset(sArr, 0, sizeof(int));       // initialize to 0

    // create bSize amount of threads and call baggage function
    for(int i = 0; i < bSize; i++) {
        pthread_create(&b[i], NULL, (void *)baggage, NULL);
    }

    // create sSize amount of threads and call security function
    for(int i = 0; i < sSize; i++) {
        pthread_create(&s[i], NULL, (void *)security, NULL);
    }

    // create fSize amount of threads and call flight function
    for(int i = 0; i < fSize; i++) {
        pthread_create(&f[i], NULL, (void *)flight, NULL);
    }

    // create pSize amount of threads, make arguments = 1...pSize + 1, and call terminal function with each arg
    for(int i = 0; i < pSize; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i + 1;
        pthread_create(&p[i], NULL, (void *)terminal, arg);
    }

    // for each passenger place them (signal) in the waiting line
    for (int i = 0; i < pSize; i++){
        sem_post(&waitSem);
    }

    // join the baggage threads
    for(int i = 0; i < bSize; i++) {
        pthread_join(b[i], NULL);
    }

    // join the security threads
    for(int i = 0; i < sSize; i++) {
        pthread_join(s[i], NULL);
    }

    // join the flight threads
    for(int i = 0; i < fSize; i++) {
        pthread_join(f[i], NULL);
    }

    // join the passenger threads
    for(int i = 0; i < pSize; i++) {
        pthread_join(p[i], NULL);
    }

    sem_destroy(&pSem);             // destroy passenger semaphore
    sem_destroy(&bSem);             // destroy baggage semaphore
    sem_destroy(&sSem);             // destroy security semaphore
    sem_destroy(&fSem);             // destroy flight semaphore
    sem_destroy(&waitSem);          // destroy wait line semaphore
    pthread_mutex_destroy(&pLock);  // destroy passenger mutex
    pthread_mutex_destroy(&bLock);  // destroy baggage mutex
    pthread_mutex_destroy(&sLock);  // destroy security mutex
    pthread_mutex_destroy(&fLock);  // destroy flight mutex

    // print a message the airplane is boarded
    printf("\n** The airplane is fully boarded and ready for flight. **\n\n");

    return 0;
}