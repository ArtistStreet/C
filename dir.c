#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct node {
    char name[100]; // fix variable name
    int isDir; // 1 is dir, 0 is file
    struct node *parent;
    struct node *child;
    struct node *next; // manage file or dir in the same dir
} node;

typedef struct file_system {
    node *root; // root node
    node *current; // current node
} file_system;

file_system *init() {
    file_system *fs = (file_system *)malloc(sizeof(file_system)); // allocate memory for file_system
    node *root = (node *)malloc(sizeof(node)); // allocate memory for root node
    strcpy(root->name, "/"); // set root name to "/"
    root->isDir = 1; // set root is directory
    root->parent = NULL;
    root->child = NULL;
    root->next = NULL;
    fs->root = root; // set root node to file_system
    fs->current = root; // set current node to root
    return fs;
}

bool check_name(const char *name, node *current) {
    node *new_node = current->child;
    while (new_node != NULL) {
        if (strcmp(name, new_node->name) == 0) { // check if name exists
            printf("Name already exists\n");
            return true;
        }
        new_node = new_node->next; // move to the next node
    }
    return false;
}

void mkdir(const char *names, file_system *fs) {
    char *name = strtok((char *)names, " ");
    while (name != NULL) {
        node *new_node = (node *)malloc(sizeof(node)); // allocate memory for new node
        strcpy(new_node->name, name); // set name
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
        name = strtok(NULL, " ");
    }
}

void ls(file_system *fs) {
    node *current = fs->current->child; // get child of current node
    while (current != NULL) {
        if (current->isDir) {
            printf("\033[1;31m%s\033[0m ", current->name); // print directory in red
        } else {
            printf("\033[1;34m%s\033[0m ", current->name); // print file in blue
        }
        current = current->next; // get next node
    }
    if (current != fs->current->child) 
        printf("\n");
}

void touch(const char *names, file_system *fs) {
    char *name = strtok((char *)names, " "); // get first name
    while (name != NULL) {
        if (check_name(name, fs->current) == false) { //  
            node *new_node = (node *)malloc(sizeof(node));
            strcpy(new_node->name, name);
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
            name = strtok(NULL, " "); // get next name
        }
        else {
            return; 
        }
    }
}

int main() {
    file_system *fs = init(); 
    printf("Root: %s\n", fs->root->name);
    char choice[100], names[100];

    while (1) {
        scanf("%s", choice);

        if (strcmp(choice, "mkdir") == 0) {
            scanf(" %[^\n]", names); // read until the next line
            mkdir(names, fs);
        } else if (strcmp(choice, "ls") == 0) {
            ls(fs);
        } else if (strcmp(choice, "touch") == 0) {
            scanf(" %[^\n]", names); // read the entire line for multiple file names
            touch(names, fs);
        } 
        
        else {
            printf("%s: command not found\n", choice);
            return 1;
        }
    }

    return 0; 
}