#include <iostream>     // for std::cin/cout
#include <string>       // for strings
#include <fcntl.h>      // open() Posix READONLY et...
#include <unistd.h>     // for read() and write()
#include <sys/stat.h>   // for struct stat and info.st_size

std::string deCompress(char* buf, size_t len) {
    std::string newBuf = "";    // initialize string to store decompressed data
    
    // look for the char '-' or '+', 
    // if found then convert the following number (i.e. 26) to a string,
    // then convert the string (26) to an int 
    // and store that many (26) 1's or 0's (depending if found a '-' or '+')
    // into the new string initialized above (newBuff)
    // if no '-' or '+' found, then just store the 1's or 0's as is
    for (size_t i = 0; i < len; i++) {
        if (buf[i] == '-') {
            i++;
            std::string temp = "";
            while (buf[i] != '-') {
                temp += buf[i];
                i++;
            }
            for (int i = 0; i < stoi(temp); i++) {
                newBuf += '0';
            }
        }
        else if (buf[i] == '+') {
            i++;
            std::string temp = "";
            while (buf[i] != '+') {
                temp += buf[i];
                i++;
            }
            for (int i = 0; i < stoi(temp); i++) {
                newBuf += '1';
            }
        }
        else {
            newBuf += buf[i];
        }
    }
    return newBuf; // return the string
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


    size = read(source, buf, info.st_size);     // read source.txt
    std::string final = deCompress(buf, size);  // use a string "final" to store the string from decompress function
    char newBuf[final.length() + 1];            // initialize char array to store the string b/c write() requires char array
    strcpy(newBuf, final.c_str());              // copy string "final" to the new char arry (in order to use write())
    write(dest, newBuf, final.length());        // write everything to the destination.txt

    close(source);  // close source.txt
    close(dest);    // close destination.txt
}