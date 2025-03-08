#include "lib.h"

bool check_name(const char *name, node *current) {
    node *new_node = current->child;
    while (new_node != NULL) {
        if (strcmp(name, new_node->name) == 0) { // check if name exists
            printf("cannot create '%s': name already exists\n", name);
            return true;
        }
        new_node = new_node->next; // move to the next node
    }

    return false;
}

void mkdir(const char *names, file_system *fs) {
    while (names != NULL) {
        if (check_name(names, fs->current) == false) { // check if name exists
            node *new_node = (node *)malloc(sizeof(node)); // allocate memory for new node
            strcpy(new_node->name, names); // set name
            new_node->isDir = 1; // set is dir
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
            names = strtok(NULL, " ");
        }
        else {
            return; 
        }
    }
}