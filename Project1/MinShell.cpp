#include <iostream> // for std::cout and cin
#include <unistd.h> //for fork and execvp functions

int main() {

    char command[BUFSIZ]; // array to store user command, BUFSIZ is 1024 bytes

    // while the word "exit" is not typed, continue the shell program
    while(strcmp(command, "exit") != 0) {
        
        std::cout << "<MiniShell>";         // output <MiniShell> 
        std::cin.getline(command, BUFSIZ);  // get the command from the user, store in command array
        
        // if the word "exit" is entered, exit the loop
        if (strcmp(command, "exit") == 0)
            break;

        pid_t pid = fork(); // fork process

        // if fork fails, issue error and exit
        if (pid < 0) {
        std::cout << "Process creation failed." << "\n";
        exit(1);
        }
        else if (pid == 0) {        // child process
            char* argv[2];          // initialize variable to store commands and issue to exec call
		    argv[0] = command;      // first index of argv will store entire command
		    argv[1] = NULL;         // final index will be NULL
            execvp(command, argv);  // issue execvp command
            std::cout << "'" << command << "' is not a command.\n"; // if it gets here then command is not correct
            exit(1);    // exit 
        }
        else {
            wait(&pid); // wait for child process
        }
    }
    
    return 0;
}