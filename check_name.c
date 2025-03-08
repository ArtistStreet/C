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