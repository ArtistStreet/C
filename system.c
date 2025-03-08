#include "lib.h"

file_system *init() {
    file_system *fs = (file_system *)malloc(sizeof(file_system)); // allocate memory for file_system
    node *root = (node *)malloc(sizeof(node)); // allocate memory for root node
    strcpy(root->name, "~"); // set root name to "/"
    root->isDir = 1; // set root is directory
    root->parent = NULL;
    root->child = NULL;
    root->next = NULL;
    fs->root = root; // set root node to file_system
    fs->current = root; // set current node to root
    return fs;
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

    if (strcmp(root->name, name) == 0) return root; // return location of root node

    node *child = find_node(root->child, name); // find in child
    if (child != NULL) return child;

    return find_node(root->next, name); // find in sibling
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
        n->parent = find_node(root, parent);
        // printf("%s\n", root);
    }

    if (n->parent != NULL) { // add child or sibling
        if (!n->parent->child) { // check if child exists
            n->parent->child = n; // set child
        } else {
            node *sibling = n->parent->child; // get first child
            while (sibling->next) { // find the last sibling
                sibling = sibling->next;
            }
            // printf("%s\n", sibling->name);
            sibling->next = n; // set next sibling to the end of the list 
        }
    }
    return n;
}

void save_current_cursor(file_system *fs) {
    FILE *file = fopen("current_location.txt", "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    node *current = fs->current;
    char path[1000] = "";
    while (current != fs->root) {
        char temp[200];
        snprintf(temp, sizeof(temp), "/%s", current->name);
        strcat(temp, path);
        strcpy(path, temp);
        current = current->parent;
    }
    fprintf(file, "~%s\n", path);
    fclose(file);
}

node *load_current_cursor(file_system *fs) {
    FILE *file = fopen("current_location.txt", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return fs->root;
    }

    char path[1000];
    if (fscanf(file, "%s", path) == 1) {
        char *token = strtok(path, "/");
        fs->current = fs->root;
        while (token != NULL) {
            node *temp = fs->current->child;
            while (temp != NULL) {
                if (strcmp(temp->name, token) == 0) {
                    fs->current = temp;
                    break;
                }
                temp = temp->next;
            }
            token = strtok(NULL, "/");
        }
    }
    fclose(file);
    return fs->current;
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
    while (!feof(file)) { // read file 
        node *n = load_node(file, fs->root);
        if (!n) break;

        if (!fs->root) { // set root node
            fs->root = n;
        }
        last_node = n;
    }

    fs->current = load_current_cursor(fs); // set current node to root
    fclose(file);
    return fs;
}

void enableRawMode(struct termios *oldt) {
    struct termios newt;
    tcgetattr(STDIN_FILENO, oldt);
    newt = *oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void disableRawMode(struct termios *oldt) {
    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
}