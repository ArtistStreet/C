#include "lib.h"

void help() {
    printf("\033[1;31m%s\033[0m: create a directory\n", "mkdir");
    printf("\033[1;31m%s\033[0m: list files and directories\n", "ls");
    printf("\033[1;31m%s\033[0m: create a file\n", "touch");
    printf("\033[1;31m%s\033[0m: change directory\n", "cd");
    printf("\033[1;31m%s\033[0m: print working directory\n", "pwd");
    printf("\033[1;31m%s\033[0m: remove a file\n", "rm");
    printf("\033[1;31m%s\033[0m: remove a directory\n", "rmdir");
    printf("\033[1;31m%s\033[0m: move (remane) file or directory\n", "mv");
    printf("\033[1;31m%s\033[0m: exit the program\n", "exit");
}