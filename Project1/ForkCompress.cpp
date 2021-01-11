#include <unistd.h> // for execl() and fork() fucntions
#include <iostream> // for std::cout/cin

// Child function is passed source.txt and destination.txt
// and uses execl to run the MyCompress program on them
void child(char &in, char &out) {
    std::cout << "Now compressing...\n";
    execl("./MyCompress", "./MyCompress", &in, &out, NULL);
}

// The parent is called after child performs its function
void parent() {
    std::cout << "Compression finished.\n";
}

int main(int argc, char* argv[]) {

    char* inName = argv[1];     // name of the source file
    char* outName = argv[2];    // name of the destination file
    pid_t pid;                  // pid to store file description
    pid = fork();               // fork process

    // if fork fails (pid < 0), print error and exit
    // if fork is successful, then the parent uses wait(), to wait on child process
    // in child process, the child function is called, 
    // afterwards, the parent function is called
    if (pid < 0) {
        std::cout << "Process creation failed." << "\n";
        exit(1);
    }
    else if (pid == 0) {
        child(*inName, *outName);
    }
    else {
        std::cout << "Wait for compression.\n";
        wait(&pid);
        parent();
    }

    return 0;
}