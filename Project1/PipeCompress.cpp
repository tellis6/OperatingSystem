#include <iostream>     // for std::cin/cout
#include <string>       // for strings
#include <fcntl.h>      // open() Posix READONLY et...
#include <unistd.h>     // for read() and write()
#include <sys/stat.h>   // for struct stat and info.st_size

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
        counter = 1; //reset counter
        }
    }
    return newBuf; //return the new string
}

// main function, takes command line argumenst for the txt files to be manipulated
int main(int argc, char* argv[]) {

    char* inName = argv[1];     // name of the source file
    char* outName = argv[2];    // name of the destination file
    pid_t pid;                  // pid to store file description
    int p[2];                   // pipe array
    struct stat info;           // initialize to use the st_size function which counts bits 
    stat(argv[1], &info);       // pass into stat the file to be read/counted
    char buf[info.st_size];     // initialize a buffer with the size of the source file
    size_t size;                // size will be used to store the file that is read() -- source.txt

    // Open source.txt in read-only, 0444 gives read permission to everyone
    int source = open(inName, O_RDONLY, 0);   

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
    
    size = read(source, buf, info.st_size); // read source.txt
    std::string final = compress(buf, size);// use a string "final" to store the string from compress function
    char newBuf[final.length()+ 1];         // initialize char array to store the string b/c write() requires char array
    strcpy(newBuf, final.c_str());          // copy string "final" to the new char arry (in order to use write())

    // If pipe can't open, issue error and exit
    if(pipe(p) == -1) {
        std::cout << "Failed to create the pipe. \n";
        exit(1);
    }
    
    pid = fork(); // fork process
    
    //Issue error if fork process fails and exit
    if(pid < 0) {
        std::cout << "Failed to create the child process. \n";
        exit(1);
    } 
   
    //Child process
    else if (pid == 0) {
        close(p[1]);                            // Close write pipe
        read(p[0], newBuf, 1);                  // open read pipe and read to the newBuff just 1 character
        write(dest, newBuf, final.length());    // write this 1 character to destination.txt
        close(p[0]);                            // Close the read pipe
        wait(&pid);                             // wait for the parent process to send the next character
        close(dest);                            //close the destination.txt
    } 
    
    //Parent process. Read source txt and compress to string, then make char array and write to pipe, one char at a time
    else {
        int i = 0; // initialize index to sort through entire source.txt, length is final.length()
        
        // while iterating through each char, orginially from source.txt
        while (i < final.length()+ 1) {     
            close(p[0]);                        // close the read end of the pipe
            write(p[1], &newBuf[i], 1);         // open write pipe and write to the newBuff just 1 character
            close(p[1]);                        // close the write end of the pipe
            i++;                                // increment the index
            wait(&pid);                         //wait for the child process
        } 
    }
    close(source); //close the source.txt
    return 0;
}