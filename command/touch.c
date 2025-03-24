#include "lib.h"

void touch(const char *names, file_system *fs) {
    while (names != NULL) {
        if (check_name(names, fs->current) == false) { //  
            node *new_node = (node *)malloc(sizeof(node));
            strcpy(new_node->name, names);
            new_node->isDir = 0; // set is dir
            new_node->parent = fs->current; // set parent
            new_node->child = NULL;
            new_node->next = NULL;

            if (fs->current->child == NULL) {
                fs->current->child = new_node; // set child if no child exists
            } else {
                node *sibling = fs->current->child;
                while (sibling->next != NULL) {
                    sibling = sibling->next; // find the last sibling
                }
                sibling->next = new_node; // add new node as the next sibling
            }
            names = strtok(NULL, " "); // get next name
        }
        else {
            return;
        }
    }
}