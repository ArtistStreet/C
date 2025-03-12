#include "lib.h"

char *check_string(char *string, bool *check_string_end, char **before_slash) {
    if (string == NULL || strlen(string) == 0) {
        return NULL;
    }
    // printf("%c\n", string[strlen(string) - 1]);
    char *last_slash;
    
    if (string[strlen(string) - 1] == '/') {  // if end with '/'
        string[strlen(string) - 1] = '\0'; 
        *check_string_end = true;
    }
    
    last_slash = strrchr(string, '/'); 
    if (last_slash != NULL) {
        *last_slash = '\0';
        *before_slash = strrchr(string, '/');
        if (*before_slash != NULL) {
            *before_slash += 1;
        } else {
            *before_slash = string;
        }
        return last_slash + 1;
    }
    return string;
}

typedef struct {
    char *first;
    uint8_t second;
} pair;

pair *path(file_system *fs, const char *path, uint8_t *cnt) {
    char name[100];
    strcpy(name, path);
    // printf("%s\n", name);
    node *target = find_node(fs->root, (strcmp(fs->root->name, path) == 0) ? path : name);

    if (!target || !target->isDir) {
        printf("ls: cannot access '%s': No such directory\n", path);
        return NULL;
    }

    node *current = target->child; 
    int i = 0;

    while (current != NULL) {
        i++;
        current = current->next;
    }

    if (i == 0) {
        return NULL;
    }

    pair *list = (pair *)malloc(i * sizeof(pair));
    if (!list) {
        printf("Memory allcation failed\n");
        return NULL;
    }

    current = target->child;
    i = 0;
    while (current != NULL) {
        list[i].first = strdup(current->name);
        list[i].second = (current->isDir == 0) ? 0 : 1;
        // printf("%s %ld\n", list[i].first, list[i].second);
        i++;
        current = current->next;
    }
    *cnt = i;
    return list;
}

void free_memory() {

}

char *handle_tab(file_system *fs, char *res, char *before_slash, uint8_t cnt_tab, uint8_t *loop) {
    node *current = fs->current->child;
    char *result = NULL;
    uint8_t cnt = 0, count = 0;
    // printf("123");
    pair *list = path(fs, before_slash, &cnt);
    pair *l = (pair *)malloc(100 * sizeof(pair));
    if (cnt_tab == 1) {
        printf("\n");
        uint8_t index = 0;
        for (size_t i = 0; i < cnt; i++) {
            if (hash_for_input(res) == hash_for_file(list[i].first, strlen(res))) {
                l[index].first = strdup(list[i].first); 
                l[index].second = list[i].second; 
                // printf("%s ", l[index].first);
                index++; 
            }
        }

        if (index == 1) {
            printf("%s\n", l[0].first);
            return strdup(l[0].first);
        }

        for (size_t i = 0; i < index; i++) {
            if (l[i].first != NULL) {
                printf("%d", l[i].second);
            }
        }


        // if () {
        //     printf("%s\n", list[0].first);
        //     return strdup(list[0].first);
        // }
    } else {
        printf("\r\033[K");
        printf("\033[s");
        for (size_t i = 0; i < cnt; i++) {
            if (i == *loop % cnt) {
                printf("\033[1;35m%s\033[0m ", list[i].first);
                result = strdup(list[i].first);
            } else {
                printf("%s ", list[i].first);
            }
        }
        printf("\033[K");
        printf("\033[u");
        fflush(stdout);
        (*loop)++;
    }

    return result ? result : NULL;
}