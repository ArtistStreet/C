#include "lib.h"

char *check_string(char *string, bool *check_string_end, char **before_slash) {
    if (string == NULL || strlen(string) == 0) {
        return NULL;
    }
    // printf("%c\n", string[strlen(string) - 1]);
    char *last_slash, *split;
    
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

char **path(file_system *fs, const char *path, uint8_t *cnt) {
    char name[100];
    strcpy(name, path);
    printf("%s\n", name);
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

    char **list = (char **)malloc(i * sizeof(char *));
    if (!list) {
        printf("Memory allcation failed\n");
        return NULL;
    }

    current = target->child;
    int index = 0;
    while (current != NULL) {
        list[i] = strdup(current->name);
        i++;
        current = current->next;
    }

    *cnt = i;
    return list;
}

char *handle_tab(file_system *fs, char *res, char *before_slash, uint8_t cnt_tab, uint8_t *loop) {
    node *current = fs->current->child;
    char *result = NULL;
    uint8_t cnt = 0;
    printf("%s\n", before_slash);
    char **list = path(fs, before_slash, &cnt);
    printf("%d\n", cnt);
    // for (size_t i = 0; i < cnt; i++) {
    //     printf("%s ", list[i]);
    // }
    // if (cnt == 1) { // if only 1 file exists return itself
    //     // printf(save_file_with_similar_name[0]);   
    //     result = strdup(save_file_with_similar_name[0]);
    //     return result;
    // } else if (cnt > 1) {
    //     if (cnt_tab > 1) {
    //         printf("\r\033[K"); 
    //         printf("cd %s", save_file_with_similar_name[*loop % cnt]);
    //         fflush(stdout);  
            
    //         printf("\033[s");  // save current location        
    //         printf("\n");

    //         for (size_t i = 0; i < cnt; i++) {
    //             if (i == *loop % cnt) {
    //                 printf("\033[1;32m%s\033[0m ", save_file_with_similar_name[i]); // In màu xanh thư mục được chọn
    //                 memset(save_tab + 3, 0x0, sizeof(save_tab) - 3);
    //                 strcat(save_tab, save_file_with_similar_name[i]);
    //             } else {
    //                 printf("%s ", save_file_with_similar_name[i]);
    //             }
    //         }
            
    //         printf("\033[K");
    //         printf("\033[u"); // restore cursor
    //         fflush(stdout);
            
    //         (*loop)++;
    //     }
    // }
    // }
    // }
    // return result ? result : NULL;
}