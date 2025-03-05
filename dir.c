#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char name[100];
    int isDir; // 1 is dir, 0 is file
    struct node *parent;
    struct node *child;
    struct node *next;
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

void mkdir(const char *name, file_system *fs) {
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
}

void ls(file_system *fs) {
    node *current = fs->current->child; // get child of currnent node
    while (current != NULL) {
        printf("%s ", current->name);
        current = current->next; // get next node
    }
}

int main() {
    file_system *fs = init(); 
    printf("Root: %s\n", fs->root->name);
    char choice[100], name[100];

    while (1) {
        scanf("%s", choice);

        if (strcmp(choice, "mkdir") == 0) {
            scanf("%s", name);
            mkdir(name, fs);
        } else if (strcmp(choice, "ls") == 0) {
            ls(fs);
        } else {
            printf("%s: command not found\n", choice);
            return 1;
        }
    }

    return 0; 
}