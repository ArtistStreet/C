#include "lib.h"

void ls(file_system *fs) {
    node *current = fs->current->child; // get child of current node
    while (current != NULL) {
        if (current->isDir) {
            printf("\033[1;32m%s\033[0m ", current->name); // print directory in red
        } else {
            printf("\033[1;35m%s\033[0m ", current->name); // print file in blue
        }
        current = current->next; // get next node
    }
    if (current != fs->current->child) 
        printf("\n");
}