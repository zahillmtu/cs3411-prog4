/**
 * Zachary Hill
 * CS3411
 * prog4 - Shell
 * 3/17/2016
 */
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<readline/readline.h>
#include<readline/history.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

#define GRN  "\001\x1B[32m\002" // for the prompt
#define NRM  "\001\x1B[0m\002" // to set the color back to normal


/**
 * Function to fork a process and run the execvp command. exec should be set
 * to the name of the executable and args should contain an array where arg[0]
 * is the name of the exec and arg[1 - 31] are the arguments passed
 */
void run(char *exec, char *args[]) {

    // fork the process
    pid_t pid = fork();
    if(pid == -1) {
        perror("fork");
        return;
    }
    else if(pid == 0) {
        // The child process
        // Call execvp
        if (execvp(exec, args) < 0) {
            printf("An error occured running the program %s - Try again\n", exec);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        // wait for the child
        int status = 0;
        wait(&status);
        return;
    }
    return;
}

/**
 * This is the main function that runs the control for the mtush shell
 * It will run in a loop collecting input and calculating results until
 * the user types "exit".
 */
int main(void) {

    //int retVal;

    while(1) {

        // get the current working directory
        char *cwd = getcwd(NULL, 0);
        if (cwd == NULL) {
            perror(cwd);
            exit(EXIT_FAILURE);
        }

        char *dest = (char*) malloc(sizeof(char)*1024);
        //sprintf(dest, GRN); // put in green
        sprintf(dest, "mtush %s> ", cwd); // put in value
        //strncat(dest,

        //retVal = sprintf(noColor, "mtush %s> ", cwd);
        //if (retVal < 0) {
        //    printf("Error writing to string\n");
        //    exit(EXIT_FAILURE);
        //}
        //char* prompt = GRN;
        //prompt = strcat(prompt, noColor);
        //prompt = strcat(prompt, NRM);

        // read in the user input
        char *line = readline(dest);
        if (!line) {
            printf("Error reading input, please try again\n");
            continue;
        }
        else { // Do stuff with the input
            // if the input is exit return
            printf("This is line: %s\n", line);
            if (strcmp(line, "exit") == 0) {
                free(line);
                free(dest);
                exit(EXIT_SUCCESS);
            }
            else {
                char *toks = strtok(line, " ");
                // run the command given
                if (toks != NULL) {
                    // set the executable
                    char *exec = toks;
                    char *args[64] = { NULL }; // make sure array is null terminated
                    args[0] = exec; // first value must be set to program name
                    int j = 1;
                    // call again to get the next piece of the string
                    toks = strtok(NULL, " ");
                    while (toks != NULL) {
                        args[j] = toks;
                        toks = strtok(NULL, " ");
                    }
                    // run the exec command
                    run(exec, args);
                }
                printf("You entered: %s\n", line);
                free(line);
                free(dest);
            }
        }
    }
    exit(EXIT_FAILURE);
} // end of main

/**
 * References
 * strtok - http://stackoverflow.com/questions/9629473/c-extracting-words-from-string
 */
