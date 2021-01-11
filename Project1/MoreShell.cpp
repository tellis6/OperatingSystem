#include <iostream> // for std::cout and cin
#include <unistd.h> // for fork, execvp, and other posix functions

int main() {

    char command[BUFSIZ];   // command array from the user input, BUFSIZ is 1024 bytes
    char *cmdAr[BUFSIZ];    // array of words entered with space between, BUFSIZ is 1024 bytes

    // while the word "exit" is not typed, continue the shell program
    while(strcmp(command, "exit") != 0) {
        
        std::cout << "<MiniShell>";         // output <MiniShell> 
        std::cin.getline(command, BUFSIZ);  // get the command from the user, store in command array
        
        // if the word "exit" is entered, exit the loop
        if (strcmp(command, "exit") == 0)
            break;

        int cmdLen = 0;                     // counter, will count how long the command is
        char* token = strtok(command, " "); // tokenize, split the command by words...space is the delimiter
        
        // while iterating through the command, increment the cmdLen counter, 
        // to determine size of the command, and add each word to the cmdAr array
        while (token&&strcmp(token,"|")!=0) {
            cmdAr[cmdLen] = token;
            cmdLen++;
            token = strtok(NULL, " ");
        }
        
        pid_t pid = fork(); // fork process

        // issue error if fork process fails and exit.
        if (pid < 0) {
            std::cout << "Process creation failed." << "\n";
        }
        else if (pid == 0) {        // if fork successfull, this is child process
            char* argv[cmdLen];     // initialize variable to store commands and issue to exec call
            char* cmd = cmdAr[0];   // variable cmd will hold the first index of cmdAr (main command)

            // interate through the rest of the cmdAr 
            // and assign each command to the argv array
            for (int i = 0; i < cmdLen; i++) {
                argv[i] = cmdAr[i];
            }
            argv[cmdLen] = NULL; // final index will be set to NULL

            execvp(cmd, argv);                          // issue command
            std::cout << "This is not a command.\n";    // if here then command failed, issue error and exit
            exit(1);
        }
        else {
            wait(&pid); // wait for child process
        }
    }
    return 0;
}