#include <unistd.h> // for dup2, execvp, close, and other posix functions
#include <iostream> // for std::cout and cin

int main() {

    char command[BUFSIZ];   // command array from the user input, BUFSIZ is 1024 bytes
    char *cmdAr[BUFSIZ];    // array of words entered with space between, BUFSIZ is 1024 bytes
    int p[2];               // pipe array
    pid_t pidA;             // first pid, file description from first fork
    pid_t pidB;             // second pid, file description from second fork
    std::string left = "";  // will store the command from left side of the pipe '|'
    std::string right = ""; // will store the command from right side of the pipe '|'

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
        while (token) {
            cmdAr[cmdLen] = token;
            cmdLen++;
            token = strtok(NULL, " ");
        }

        int cmdL = 0;   // command to count the length of the command on the left of the pipe "|"
        int cmdR = 0;   // command to count the length of the command on the right of the pipe "|"
        int i = 0;      // index to help iteration

        // while iterating through the entire command length (cmdLen),
        // count the lengths to the left and right of the pipe "|"
        while (i < cmdLen) {
            if (strcmp(cmdAr[i],"|")==0) {
                i++;
                while (i < cmdLen) {
                    cmdR++;
                    i++;
                }
            }
            else {
                cmdL++;
                i++;
            }
        }

        char* argvL[cmdL+1];                    // initialize array of left side commands to use in our execvp call
        char* cmd1 = cmdAr[0];                  // first index of cmdAr is set to variable cmd1

        // for all commands on the left of the pipe, 
        // place in argvL array
        for (int i = 0; i < cmdL; i++) {    
            argvL[i] = cmdAr[i];
        }
        argvL[cmdL] = NULL; // final index of argvL array is set to null

        char* argvR[cmdR+1];                    // initialize array of right side commands to use in our execvp call
        char* cmd2 = cmdAr[cmdL+1];             // index following the pipe of cmdAr (cmdL+1) is set to variable cmd2
        int j = cmdL+1;                         // first index number after pipe is stored in variable j
        int k = 0;                              // k is first index 0
        
        // while iterating through the right side of cmdAr, 
        // store the cmdAr command into the argvR array
        while (j < cmdLen || k < cmdR) {
            argvR[k] = cmdAr[j];
            j++;
            k++;
        }
        argvR[cmdR] = NULL; // final index of argvR array is set to null

        // If pipe can't open, issue error and exit
        if(pipe(p) == -1) {
        std::cout << "Failed to create the pipe. \n";
        exit(1);
        }

        pidA = fork(); // fork process

        // issue error if fork process fails and exit.
        if(pidA < 0) {
        std::cout << "Failed to create the child process. \n";
        exit(1);
        }
        else if(pidA == 0) {                    // if fork successfull, this is child process
            dup2(p[1], STDOUT_FILENO);          // reroute output to write end of pipe
            close(p[0]);                        // close read end of pipe
            int status = execvp(cmd1, argvL);   // execute command from left side of pipe, and set status (fail or success)
            
            // if unsuccessulf issue error and try to get a different command
            if (status == -1){
                std::cout << "This is not a command.\n";
                exit(1);
            }
        }
        else {
            pidB = fork(); // second fork process
            
            // if second fork is unsuccessful then issue error and exit
            if(pidB < 0) {
                std::cout << "Failed to create the child process. \n";
                exit(1);
            }
            else if(pidB == 0) {                    // if successful this is grandchild
                dup2(p[0], STDIN_FILENO);           // redirect input to read end of pipe
                close(p[1]);                        // close write end of pipe
                int status = execvp(cmd2, argvR);   // execute command from right side of pipe, and set status (fail or success)
                close(p[0]);                        // close read end of pipe
                close(p[1]);                        // close write end of pipe

                // if unsuccessulf issue error and try to get a different command
                if (status == -1){
                    std::cout << "This is not a command.\n";
                    exit(1);
                }
            }
            else {
                wait(&pidB);    // wait for grandchild
            }
            close(p[0]);        // close read end of pipe
            close(p[1]);        //close write end of pipe
            wait(&pidA);        // wait for child process
        }
        
    } 
    return 0;
}