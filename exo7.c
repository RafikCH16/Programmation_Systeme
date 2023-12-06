#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>

#define SIZE 100

char *executeCommand()
{
    const char *MSGINPUT = "enseash%% ";

    char *cmd = (char *)malloc(SIZE * sizeof(char));
    if (cmd == NULL)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    if (write(STDOUT_FILENO, MSGINPUT, strlen(MSGINPUT)) == -1)
    {
        perror("Error writing to the terminal");
        free(cmd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead = read(STDIN_FILENO, cmd, SIZE);
    if (bytesRead == -1)
    {
        perror("Error reading from the terminal");
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
        pid_t child_pid = fork();

        if (child_pid == -1)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        // child process
        clock_gettime(CLOCK_MONOTONIC, &t_init_child); // get start time for child
        if (child_pid == 0)
        {
            printf("pid_child=%d\n", getpid());
            sleep(2);

            // Parse for redirection
            char *input_redirection = strchr(command, '<');
            char *output_redirection = strchr(command, '>');

            if (input_redirection != NULL)
            {
                *input_redirection = '\0'; // Separate the command and input file
                input_redirection++;       // Move to the input file name
                int input_fd = open(input_redirection, O_RDONLY);
                if (input_fd == -1)
                {
                    perror("Error opening input file");
                    free(command);
                    exit(EXIT_FAILURE);
                }
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }

            if (output_redirection != NULL)
            {
                *output_redirection = '\0'; // Separate the command and output file
                output_redirection++;       // Move to the output file name
                int output_fd = open(output_redirection, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (output_fd == -1)
                {
                    perror("Error opening output file");
                    free(command);
                    exit(EXIT_FAILURE);
                }
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
            }

            if (strcmp(command, "exit") == 0 || command[0] == '\0')
            {
                write(STDOUT_FILENO, "Bye bye...\n", strlen("Bye bye...\n"));
                free(command);
                exit(EXIT_SUCCESS);
            }

            if (execlp("/bin/sh", "/bin/sh", "-c", command, NULL) == -1)
            {
                perror(ERRORMSG);
                free(command);
                exit(EXIT_FAILURE);
            }
        }

        // parent process
        else
        {
            int status;
            wait(&status);
            clock_gettime(CLOCK_MONOTONIC, &t_finish_child); // get finish time for child

            double finish = (t_finish_child.tv_sec - t_init_child.tv_sec) * 1000.0 + (t_finish_child.tv_nsec - t_init_child.tv_nsec) / 1000000.0; // Calculate the elapsed time in milliseconds

            char msgState[100] = "\0";

            // test if the process completed correctly or if there is an interruption
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

            // Note: Do not free(command) here to avoid double-freeing the memory
        }
    }

    return 0;
}
