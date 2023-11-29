#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
void displayPrompt()
{
    const char welcomeMessage[] = "Bienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.\nenseash %%\n";
    if (write(STDOUT_FILENO, welcomeMessage, sizeof(welcomeMessage) - 1) == -1)
    {
        perror("Error writing to the terminal");
    }
}

int main()
{
    // juste display a welcomeMessage message
    displayPrompt();
}
