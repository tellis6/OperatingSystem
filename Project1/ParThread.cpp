#include <stdio.h>
#include <pthread.h>
#include <unistd.h>    // for fork(), read() and other posix functions
#include <sys/stat.h>  // for struct stat and info.st_size
#include <iostream>    // for std::cout and cin
#include <fcntl.h>     // for open() and close() functions
#include <sys/time.h>  // for the gettimeofday timer/clock
#include <string>

//create a struct so you can pass to thread function
struct dataStruct {
    std::string data;	// string to store the chuck of data
    int size;		    // variable to store the size of data
};

std::string compress(std::string buf, size_t len) {
    int counter = 1;            // set counter
    std::string newBuf = "";    // initialize string to store compressed data

    // convert current and next chars to int and subtract, 
    // if they equal 0 they are the same number so inc counter.
    // if they are not the same number, then check counter.
    // if counter smaller than 16, then add the nums to the new string
    // as they are. Otherwise, if counter > 16, then "compress"
    for(size_t i=0; i < len; i++) {
        if((((int)buf[i]) - (int)(buf[i+1])) == 0) {
            counter++;
        }
        else {
            if (counter < 16) {
                char temp = buf[i];
                for (int i = 0; i < counter; i++) {
                    newBuf = newBuf + temp;
                }
            }
            else {
                if ((buf[i]) == '1') {
                    newBuf = newBuf + '+' + std::to_string(counter) + '+';
                }
                else {
                    newBuf = newBuf + '-' + std::to_string(counter) + '-';
                }                
            }
        counter = 1; // reset counter
        }
    }
    return newBuf; // return the new string
}

void *thread(void *data) {
    struct dataStruct *args = (struct dataStruct *)data;    // create a pointer to the struct
    args->data = compress(args->data, args->size);          // set the compressed data to the struct data
    pthread_exit(NULL);                                     //exit pthread
}

// main function, takes command line argumenst for the txt files to be manipulated
int main(int argc, char* argv[]) {
	char* inName = argv[1];			// name of the source file
    char* outName = argv[2];	    // name of the destination file
    struct stat info;           	// initialize to use the st_size function which counts bits 
    stat(argv[1], &info);       	// pass into stat the file to be read/counted
    char buf[info.st_size];     	// initialize a buffer with the size of the source file
    size_t size;                	// size will be used to store the file that is read() -- source.txt
    size_t newSize;             	// this will be the new size when divided into n parts
    int n = 5;                  	// minimum number of forks
    struct timeval begin, stop; 	// structs to store time
    gettimeofday(&begin, 0);    	// get start time
	pthread_t thread_id[n];         // thread_ids
	struct dataStruct argD[n];      // struct to send arguments to thread() function
    std::string final = "";         // string to store the final data to, in order to write to destination

	// Open source.txt in read-only, 0444 gives read permission to everyone
    int source = open(inName, O_RDONLY, 0);

	// If it can't open, it sends message and exits
    if (source == -1) {
        std::cout << "The file " << inName << " does not exist. \n";
        exit(1);
    }

	size = read(source, buf, info.st_size); // read source.txt 
    close(source);                          // close the source.txt

	// if size divides evenly by n, then newSize will be the divided size,
    // otherwise new size will be 1 more than the floor of new size divided by n
    if (size % n != 0) {
        newSize = floor(size/n) + 1;
    }
    else {
        newSize = size/n;
    }

    // make an array of structs and initialize each value
	for(int i = 0; i < n; i++) {   
		argD[i].data = "";   
		argD[i].size = newSize;
	}

    // load the data in struct, with each chunk from buf
    // first do the 0 index, and then do the rest of the indexes
	for (int i = 0; i < newSize; i++) {
		argD[0].data += buf[i];
	}
	int j = 0;
	for (int i = 1; i < n; i++) {
		j = newSize*(i);
		while (j < newSize*(i+1)) {
			argD[i].data += buf[j];
			j++;
		}
	}
	
	// for each thread call the thread function 
    // which will compress each chunk in the struct
	for(int i = 0; i < n; i++) {   
		pthread_create(&thread_id[i], NULL, &thread, (void *)&argD[i]);
	}

    // join the threads back
	for(int i = 0; i < n; i++) {
		pthread_join(thread_id[i], NULL);
	}

    // Open destination.txt in read and write mode - O_RDWR, 
    // if it doesnt exit then create the file - O_CREAT, 
    // if the file exists already, then truncate it to 0 - O_TRUNC
    // permission 0777 - anyone can read/write
    int dest = open(outName, O_RDWR | O_CREAT | O_APPEND, 0777);

    for (int i = 0; i < n; i++) {
        final = argD[i].data;                   // copy the compressed version to a string
        char newBuf[final.length() + 1];        // initialize char array to store the string b/c write() requires char array
        strcpy(newBuf, final.c_str());          // copy string "final" to the new char arry (in order to use write())
        write(dest, newBuf, final.length());    // write everything to the destination.txt
    }

    close(source);  // close source.txt
    close(dest);    // close destination.txt

	gettimeofday(&stop, 0);                                                         // get the time
    long seconds = stop.tv_sec - begin.tv_sec;                                      // take difference for seconds
    long microseconds = stop.tv_usec - begin.tv_usec;                               // add in the microseconds for precision
    double milliseconds = (seconds + microseconds*1e-6)*1000;                       // convert to milliseconds
    std::cout << "The program took: " << milliseconds << " milliseconds." << "\n";  // print time
    return 0;
}