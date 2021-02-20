#include <stdio.h>
#include <unistd.h>
#include <string>

#include "function.h"

int main(int argc, char *argv[])
{
    loadFile();
    loadProgramm(HDD[0]);
    clearConsole();
    memory[NEXT_COMMAND_MEMORY]=START_PROGRAMM_MEMORY;
    while(runOneStep())
    {
        gotoHomeConsole();
        printMemory();
        printDisplay();
//        printGraphicDisplay();
        debug();
        usleep(700);
    } 
    printMemory();
    printDisplay();
    //clearConsole();
    return 0;
}
