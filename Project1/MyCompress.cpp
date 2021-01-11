#include <iostream>     // for std::cin/cout
#include <string>       // for strings
#include <fcntl.h>      // open() Posix READONLY et...
#include <unistd.h>     // for read() and write()
#include <sys/stat.h>   // for struct stat and info.st_size
#include <sys/time.h>   // for the gettimeofday timer/clock

std::string compress(char* buf, size_t len) {
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

// main function, takes command line argumenst for the txt files to be manipulated
int main(int argc, char* argv[])
{
    char* inName = argv[1];     // name of the source file
    char* outName = argv[2];    // name of the destination file
    struct stat info;           // initialize to use the st_size function which counts bits 
    stat(argv[1], &info);       // pass into stat the file to be read/counted
    char buf[info.st_size];     // initialize a buffer with the size of the source file
    size_t size;                // size will be used to store the file that is read() -- source.txt
    struct timeval begin, stop; // structs to store time
    gettimeofday(&begin, 0);    // get start time

    // Open source.txt in read-only, 0444 gives read permission to everyone
    int source = open(inName, O_RDONLY, 0444);

    // If it can't open, it sends message and exits
    if (source == -1) {
        std::cout << "The file " << inName << " does not exist. \n";
        exit(1);
    }

    // Open destination.txt in read and write mode - O_RDWR, 
    // if it doesnt exit then create the file - O_CREAT, 
    // if the file exists already, then truncate it to 0 - O_TRUNC
    // permission 0777 - anyone can read/write
    int dest = open(outName, O_RDWR | O_CREAT | O_TRUNC, 0777);

    size = read(source, buf, info.st_size);    // read source.txt
    std::string final = compress(buf, size);   // use a string "final" to store the string from compress function
    char newBuf[final.length() + 1];           // initialize char array to store the string b/c write() requires char array
    strcpy(newBuf, final.c_str());             // copy string "final" to the new char arry (in order to use write())
    write(dest, newBuf, final.length());       // write everything to the destination.txt

    close(source);  // close source.txt
    close(dest);    // close destination.txt

    gettimeofday(&stop, 0);                                                         // get the time
    long seconds = stop.tv_sec - begin.tv_sec;                                      // take difference for seconds
    long microseconds = stop.tv_usec - begin.tv_usec;                               // add in the microseconds for precision
    double milliseconds = (seconds + microseconds*1e-6)*1000;                       // convert to milliseconds
    std::cout << "The program took: " << milliseconds << " milliseconds." << "\n";  // print time
    return 0;
}