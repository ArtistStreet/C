#include "lib.h"

void REMOVE(const char *names, file_system *fs, const int check) {
    while (names != NULL) {
        node *prev = NULL; // previous node
        node *temp = fs->current->child; // get child of current node
    
        while (temp != NULL) {
            if (check == 0) {
                if (strcmp(temp->name, names) == 0 && temp->isDir == 0) { // check if name matches and is a file
                    if (prev == NULL) {
                        fs->current->child = temp->next; // remove the first child
                    } else {
                        prev->next = temp->next; // remove the node from the list
                    }
                    free(temp); // free the memory
                    break;
                } 
                // else {
                //     printf("rm: cannot remove '%s': Is a directory\n", names);
                //     break; 
                // }
            } 
            else if (check == 1) {
                if (strcmp(temp->name, names) == 0 && temp->isDir == 1) { // check if names matches and is a file
                    if (prev == NULL) {
                        fs->current->child = temp->next; // remove the first child
                    } else {
                        prev->next = temp->next; // remove the node from the list
                    }
                    free(temp); // free the memory
                    break;
                }
                //  else if (strcmp(temp->name, names) == 0 && temp->isDir == 0){
                //     printf("rmdir: failed to remove '%s': Not a directory\n", names);
                // }
            }
            prev = temp; 
            temp = temp->next; // move to the next node
        }
        names = strtok(NULL, " "); // get next name
    }
}