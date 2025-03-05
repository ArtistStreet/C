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
    strcpy(root->name, "~/"); // set root name to "/"
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
        if (check_name(name, fs->current) == false) { //  
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
        else {
            return; 
        }
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

void cd(const char *names, file_system *fs) {
    if (strcmp(names, "..") == 0) {
        if (fs->current->parent != NULL) {
            fs->current = fs->current->parent; // move to parent
        }
        return; 
    }
    node *temp = fs->current->child; // get child of current node
    while (temp != NULL) {
        if (strcmp(names, temp->name) == 0) {
            if (temp->isDir) {
                fs->current = temp; // move to the directory;
                return;
            } else {
                printf("Not a directory\n");
                return;
            }
        }
        temp = temp->next; // get next node
    }
}

void save_node(FILE *file, node *n) {
    if (n == NULL) return;

    fprintf(file, "%s %s %d\n", n->parent ? n->parent->name : "ROOT", n->name, n->isDir);

    // Save all children first (recursive call)
    save_node(file, n->child);

    // Save the next sibling
    save_node(file, n->next);
}

void save_file_system(file_system *fs, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }
    save_node(file, fs->root);
    fclose(file);
}

node *find_node(node *root, const char *name) { // DFS
    if (root == NULL) return NULL;

    if (strcmp(root->name, name) == 0) return root; // check if name is found

    node *child = find_node(root->child, name); // find in child
    if (child != NULL) return child;

    return find_node(root->next, name);
}

node *load_node(FILE *file, node *root) {
    char name[100], parent[100];
    int isDir;
    if (fscanf(file, "%s %s %d", parent, name, &isDir) != 3) return NULL;

    node *n = (node *)malloc(sizeof(node));
    strcpy(n->name, name); // set name
    n->isDir = isDir; // set dir 
    n->child = NULL;
    n->next = NULL;

    if (strcmp(parent, "ROOT") == 0) { // check if parent if root
        n->parent = root;
    } else { // find parent node
        // printf("%s\n", n->parent->name);
        n->parent = find_node(root, parent);
    }

    if (n->parent) { // add child or sibling
        if (!n->parent->child) { // check if child exists
            n->parent->child = n; // set child
        } else {
            node *sibling = n->parent->child; // get child
            while (sibling->next) { // find the last sibling
                sibling = sibling->next;
            }
            sibling->next = n; // set next sibling
        }
    }
    return n;
}

file_system *load_file_system(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) { // check if file exists
        perror("Failed to open file");
        return init();
    }
    fseek(file, 0, SEEK_END); // move to the end of the file
    if (ftell(file) == 0) { // check if file is empty
        fclose(file);
        return init();
    }
    fseek(file, 0, SEEK_SET);
    file_system *fs = (file_system *)malloc(sizeof(file_system));
    fs->root = NULL;

    node *last_node = NULL;
    while (!feof(file)) {
        node *n = load_node(file, fs->root);
        if (!n) break;

        if (!fs->root) { // set root node
            fs->root = n;
        }
        last_node = n;
    }

    fs->current = fs->root;
    fclose(file);
    return fs;
}

int main() {
    file_system *fs = load_file_system("filesystem.txt"); 
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
        } else if (strcmp(choice, "cd") == 0) {
            scanf("%s", names);
            if (strcmp(names, "\t") == 0) {
                ls(fs);
            }
            cd(names, fs);
        } else if (strcmp(choice, "exit") == 0) {
            save_file_system(fs, "filesystem.txt");
            break;
        } else if (strcmp(choice, "pwd") == 0) {
            printf("Current: %s\n", fs->current->name);
        }
        
        else {
            printf("%s: command not found\n", choice);
            return 1;
        }
    }

    return 0; 
}