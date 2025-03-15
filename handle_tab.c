#include "lib.h"

char *check_string(char *string) {
    if (string == NULL || strlen(string) == 0) {
        return NULL;
    }
    char *last_part = strdup(string);

    if (last_part[strlen(last_part) - 1] == '/') {  // if end with '/'
        last_part[strlen(last_part) - 1] = '\0'; 
    }
    return last_part;
}

typedef struct {
    char *first;
    int8_t second;
} pair;

void free_memory(pair *list, int8_t cnt) { // free memory
    for (int8_t i = 0; i < cnt; i++) {
        free(list[i].first);
    }
    free(list);
}

pair *move(file_system *fs, const char *path, int8_t *cnt) {
    char *token = NULL;
    char name[100];
    node *original_current = fs->current; // save the original current node

    if (path[0] == '~') {
        snprintf(name, sizeof(name), "%s%s", fs->root->name, path + 1);
    } else {
        strcpy(name, path);
    }

    token = strtok(name, "/"); // tokenize the path by "/"

    if (strcmp(token, "~") == 0) {
        fs->current = fs->root;
        token = strtok(NULL, "/");
    }

    while (token != NULL) {
        node *target = find_node(fs->current, token);
        if (target != NULL) {
            if (target->isDir) {
                fs->current = target; 
            } else {
                printf("Not a directory\n");
                fs->current = original_current; // restore the original current node
                return NULL;
            }
        } else {
            printf("Directory not found\n");
            fs->current = original_current; // restore the original current node
            return NULL;
        }
        token = strtok(NULL, "/"); // get next token
    }
    
    node *current = fs->current->child; 
    pair *list = (pair *)malloc(100 * sizeof(pair));
    if (!list) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    int8_t i = 0;
    while (current != NULL) {
        list[i].first = strdup(current->name);
        list[i].second = (current->isDir == 0) ? 0 : 1;
        i++;
        current = current->next;
    }
    *cnt = i;
    fs->current = original_current; // restore the original current node
    
    return list;
}

char *handle_tab(file_system *fs, char *last_part, char *parent, int8_t cnt_tab, int8_t *loop, char *buffer, int8_t *current_location) {
    char *result = NULL;
    int8_t cnt = 0, index = 0;

    if (parent[0] == '\0') {
        parent = fs->current->name;
    }
    pair *list = move(fs, parent, &cnt);
    pair *l = (pair *)malloc(100 * sizeof(pair));
    // printf("DEBUG: last_part='%s' parent='%s'\n", last_part, parent);

    for (int8_t i = 0; i < cnt; i++) {
        if (hash_for_input(last_part) == hash_for_file(list[i].first, strlen(last_part))) {
            l[index].first = strdup(list[i].first); 
            l[index].second = list[i].second; 
            index++; 
        }
    }

    // printf("%d %d\n", cnt, index);

    if (cnt_tab == 1) {
        if (index == 1) { // return itself
            for (int8_t i = *current_location - 1; i >= 0; i--) {
                if (buffer[i] != ' ') {
                    buffer[i] = '\0';
                } 
                else {
                    break;
                }
            }
            strcat(buffer, l[0].first);
            printf("\r\033[K%s", buffer); 
            // printf("%s\n", buffer);
            // printf("234");
            free_memory(list, cnt);
            return strdup(l[0].first);
        }

        printf("\n");
        for (int8_t i = 0; i < index; i++) {
            if (l[i].second == 1) {
                printf("\033[1;32m%s\033[0m ", l[i].first);
            } else {
                printf("\033[1;35m%s\033[0m ", l[i].first);
            }
        }
        printf("\r\033[A");  // move cursor up
        printf("%s", buffer);
    } else { // return list
        printf("\r\033[K");

        printf("\n");

        for (int8_t i = 0; i < index; i++) {            
            if (i == *loop % index) {
                printf("\033[1;33m%s\033[0m ", l[i].first);
                result = strdup(l[i].first);
                strcat(buffer, l[i].first);
            } else {
                if (l[i].second == 1) {
                    printf("\033[1;32m%s\033[0m ", l[i].first);
                } else {
                    printf("\033[1;35m%s\033[0m ", l[i].first);
                }
            }
        }

        printf("\r\033[A");  // move cursor up

        printf("%s", buffer);

        fflush(stdout);

        (*loop)++;
    }

    free_memory(list, cnt);
    *current_location = strlen(buffer);
    return result ? result : NULL;
}