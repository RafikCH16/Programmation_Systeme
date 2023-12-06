#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE 100

char *executeCommand()
{
    const char *MSGINPUT = "enseash%% ";

    char *cmd = (char *)malloc(SIZE * sizeof(char));
    if (cmd == NULL)
    {
        write(STDERR_FILENO, "Memory allocation error\n", strlen("Memory allocation error\n"));
        exit(EXIT_FAILURE);
    }

    if (write(STDOUT_FILENO, MSGINPUT, strlen(MSGINPUT)) == -1)
    {
        write(STDERR_FILENO, "Error writing to the terminal\n", strlen("Error writing to the terminal\n"));
        free(cmd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead = read(STDIN_FILENO, cmd, SIZE);
    if (bytesRead == -1)
    {
        write(STDERR_FILENO, "Error reading from the terminal\n", strlen("Error reading from the terminal\n"));
        free(cmd);
        exit(EXIT_FAILURE);
    }

    // Remove the newline character from the end of the input
    size_t len = strlen(cmd);
    if (len > 0 && cmd[len - 1] == '\n')
    {
        cmd[len - 1] = '\0';
    }

    return cmd;
}

int main()
{
    const char *ERRORMSG = "Erreur: You can't execute this command\n";
    char *command;
    struct timespec t_init_child, t_finish_child;

    while (1)
    {

        command = executeCommand();
        clock_gettime(CLOCK_REALTIME, &t_init_child); // get time start child
        pid_t child_pid = fork();

        if (child_pid == -1)
        {
            write(STDERR_FILENO, "Fork failed\n", strlen("Fork failed\n"));
            exit(EXIT_FAILURE);
        }

        // child process
        if (child_pid == 0)
        {
            // Exit the loop if the command is "exit" or if Ctrl+D (EOF) is reached
            printf("pid_child=%d\n",getpid());
            sleep(10);
            if (strcmp(command, "exit") == 0 || strlen(command) == 0)
            {
                write(STDOUT_FILENO, "Bye bye...\n", strlen("Bye bye...\n"));
                free(command);
                break;
            }

            // No need for sleep(2) here
            if (execlp(command, command, NULL) == -1)
            {
                write(STDERR_FILENO, ERRORMSG, strlen(ERRORMSG));
                free(command);
                exit(EXIT_FAILURE);
            }
        }

        // parent instruction
        else
        {
            int status;
            waitpid(child_pid, &status, 0); // wait for the specific child process

            clock_gettime(CLOCK_REALTIME, &t_finish_child); // get time finish child
            double finish = (t_finish_child.tv_nsec - t_init_child.tv_nsec) / 1000000.0; // Calculate the elapsed time in milliseconds

            char msgState[100] = "\0";

            // test if the process completed correctly or if there is an interruption and specify which type of interruption presented
            if (WIFEXITED(status))
            {
                snprintf(msgState, sizeof(msgState), "enseash %% [exit : %d | %dms]\n", WEXITSTATUS(status), (int)finish);
                write(STDOUT_FILENO, msgState, strlen(msgState));
            }
            else
            {
                snprintf(msgState, sizeof(msgState), "enseash %% [sign : %d | %dms]\n", status, (int)finish);
                write(STDOUT_FILENO, msgState, strlen(msgState));
            }

            // free memory
            free(command);
        }
    }

    return 0;
}