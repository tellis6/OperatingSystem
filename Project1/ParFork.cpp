#include <unistd.h>    // for fork(), read() and other posix functions
#include <sys/stat.h>  // for struct stat and info.st_size
#include <iostream>    // for std::cout and cin
#include <fcntl.h>     // for open() and close() functions
#include <sys/time.h>  // for the gettimeofday timer/clock

void compress(char* buf, int start, int end, char &out) {

    int counter = 1;            // set counter
    std::string newBuf = "";    // initialize string to store compressed data

    // convert current and next chars to int and subtract, 
    // if they equal 0 they are the same number so inc counter.
    // if they are not the same number, then check counter.
    // if counter smaller than 16, then add the nums to the new string
    // as they are. Otherwise, if counter > 16, then "compress"
    for(int i=start; i < end+1; i++) {
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

    // Open destination.txt in read and write mode - O_RDWR, 
    // if it doesnt exit then create the file - O_CREAT, 
    // if the file exists already, then append to end - O_APPEND
    // permission 0777 - anyone can read/write
    int dest = open(&out, O_RDWR | O_CREAT | O_APPEND, 0777);

    char newBuf2[newBuf.length() + 1];      // initialize char array to store the string b/c write() requires char array
    strcpy(newBuf2, newBuf.c_str());        // copy string "newBuf" to the new char arry (in order to use write())
    write(dest, newBuf2, newBuf.length());  // write everything to the destination.txt
    close(dest);                            // close destination.txt
    exit(0);                                // exit function
}

// main function, takes command line argumenst for the txt files to be manipulated
int main(int argc, char* argv[]) {

    char* inName = argv[1];     // name of the source file
    char* outName = argv[2];    // name of the destination file
    struct stat info;           // initialize to use the st_size function which counts bits 
    stat(argv[1], &info);       // pass into stat the file to be read/counted
    char buf[info.st_size];     // initialize a buffer with the size of the source file
    size_t size;                // size will be used to store the file that is read() -- source.txt
    size_t newSize;             // this will be the new size when divided into n parts
    int start = 1;              // variable to store the start index of each compress() call
    int end = 1;                // variable to store the ending index of each compress() call
    int n = 5;                  // minimum number of forks
    struct timeval begin, stop; // structs to store time
    gettimeofday(&begin, 0);    // get start time

    // Open source.txt in read-only, 0444 gives read permission to everyone
    int source = open(inName, O_RDONLY, 0444);   

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
    
    pid_t pids[n]; // create an array of file descriptors

    // fork n processes
    for(int i = 0; i < n; i++) {
    	pids[i] = fork();

    	if (pids[i] == 0) {
            start = newSize*(i);                    // set the start index to where left off
            end = (newSize*(i+1)-1);                // set the end index to where left off
    	    compress(buf, start, end, *outName);    // call the compress function
        }
    }

    // wait for each fork to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    gettimeofday(&stop, 0);                                                         // get the time
    long seconds = stop.tv_sec - begin.tv_sec;                                      // take difference for seconds
    long microseconds = stop.tv_usec - begin.tv_usec;                               // add in the microseconds for precision
    double milliseconds = (seconds + microseconds*1e-6)*1000;                       // convert to milliseconds
    std::cout << "The program took: " << milliseconds << " milliseconds." << "\n";  // print time
    return 0; 
}