#include "lib.h"

void pwd(file_system *fs) {
    node *current = fs->current;
    char path[1000] = "";
    while (current != fs->root) {
        char temp[200];
        snprintf(temp, sizeof(temp), "/%s", current->name);
        strcat(temp, path); // add path to the temp
        strcpy(path, temp); // copy temp to path
        current = current->parent;
    }
    printf("~%s\n", path);
}