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
 * Function to tokenize the string first by "|" and second by " "
 * line should be set to the string to be tokenized, returns an
 * array containing arrays of the commands
 */
void tokenize(char *cmds[64][64], char *line) {

    char *piped[64] = { NULL }; // make sure array is null terminated

    // first seperate by pipes
    char *toks = strtok(line, "|");
    if (toks != NULL) {
        int j = 0;
        // call again to get the next piece of the string
        while (toks != NULL) {
            piped[j] = toks;
            toks = strtok(NULL, "|");
            j = j + 1;
        }
    }

    // Then seperate by spaces
    int i = 0;
    int k = 0;

    while (piped[i] != NULL) {

        toks = strtok(piped[i], " ");
        while(toks != NULL) {
            cmds[i][k] = toks;
            toks = strtok(NULL, " ");
            k = k + 1;
        }
        i = i + 1;
    }
    return;
}

/**
 * Function takes the args array and attempts to call chdir using arg[1]
 * arg[1] should be set to the desired directory and no other input should
 * be added after
 */
void callcd(char *args[]) {

    int ret;

    // Check to make sure it's just "cd" and the path
    if (args[2] != NULL) {
        printf("Error using cd - Try again\n");
        return;
    }
    // If there is no path do nothing
    if (args[1] == NULL) {
        return;
    }
    // attempt to call chdir, if it fails print a message
    ret = chdir(args[1]);
    if (ret == -1) {
        perror("chdir");
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
                char *cmds[64][64] = {{ NULL }};
                memset(cmds, 0, sizeof cmds);
                tokenize(cmds, line);

                // if there is no input just prompt again
                if (cmds[0][0] == NULL) {
                    continue;
                }
                char *exec = cmds[0][0];
                char *args[64] = { NULL };
                args[0] = exec;
                int k = 1;
                while (cmds[0][k] != NULL) {
                    args[k] = cmds[0][k];
                    k = k + 1;
                }


               // char *toks = strtok(line, " ");
               // // run the command given
               // if (toks != NULL) {
               //     // set the executable
               //     char *exec = toks;
               //     char *args[64] = { NULL }; // make sure array is null terminated
               //     args[0] = exec; // first value must be set to program name
               //     int j = 1;
               //     // call again to get the next piece of the string
               //     toks = strtok(NULL, " ");
               //     while (toks != NULL) {
               //         args[j] = toks;
               //         toks = strtok(NULL, " ");
               //         j = j + 1;
               //     }
                    // if the exec is cd, run chdir()
                    if (strcmp(exec, "cd") == 0) {
                        callcd(args);
                        continue;
                    }
                    // run the exec command
                    run(exec, args);
            }
                printf("You entered: %s\n", line);
                free(line);
                free(dest);
        }
    }
    exit(EXIT_FAILURE);
} // end of main

/**
 * References
 * strtok - http://stackoverflow.com/questions/9629473/c-extracting-words-from-string
 */
