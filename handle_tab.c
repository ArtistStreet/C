#include "lib.h"

char *check_string(char *string, bool *check_string_end, char **before_slash) {
    char *last_slash, *split;
    
    if (string[strlen(string) - 1] == '/') {  // if end with '\'
        string[strlen(string) - 1] = '\0'; 
        *check_string_end = true;
    }

    last_slash = strrchr(string, '/');  
    split = strrchr(string, '/');
    *split = '\0';
    *before_slash = strrchr(string, '/');
    *before_slash += 1;
    // printf("%s\n", before_slash);
    if (last_slash) {
        return last_slash + 1;  
    }
    return string;
}

void handle_tab(file_system *fs, char *sub, uint8_t cnt_tab, uint8_t *loop) {
    node *current = fs->current->child;
    uint8_t cnt = 0;
    // bool check_string_end = false;
    // char *before_slash;
    // // printf("%s\n", sub);
    // char *res = check_string(sub, &check_string_end, &before_slash);
    // // printf("%s\n", res);
    // if (check_string_end == true) {
    //     ls(fs, res);
    // } else {
        // printf("%s %s\n", before_slash, res);
        while (current != NULL) {
            if (hash_for_input(sub) == hash_for_file(current->name, strlen(sub)) && current->isDir == 1) {
                save_file_with_similar_name[cnt] = current->name;
                (cnt)++;
            }
            current = current->next;
        }

        if (cnt == 1) {
            printf(save_file_with_similar_name[0]);   
            // result = strdup(save_file_with_similar_name[0]);
            // return result;
        } else if (cnt > 1) {
            // if (cnt_tab == 1) {
                
            //     {
            //         printf("\n");
            //         for (size_t i = 0; i < cnt; i++) {
            //             printf("%s ", save_file_with_similar_name[i]);
            //         }
            //         printf("\033[A"); // Move cursor up
            //         printf("\r");
            //         printf("cd %s", save_file_with_similar_name[0]);
            //     }
            // } else 
            if (cnt_tab > 1) {
                printf("\r\033[K"); 
                printf("cd %s", save_file_with_similar_name[*loop % cnt]);
                fflush(stdout);  
                
                printf("\033[s");  // save current location        
                printf("\n");

                for (size_t i = 0; i < cnt; i++) {
                    if (i == *loop % cnt) {
                        printf("\033[1;32m%s\033[0m ", save_file_with_similar_name[i]); // In màu xanh thư mục được chọn
                        memset(save_tab + 3, 0x0, sizeof(save_tab) - 3);
                        strcat(save_tab, save_file_with_similar_name[i]);
                    } else {
                        printf("%s ", save_file_with_similar_name[i]);
                    }
                }
                
                printf("\033[K");
                printf("\033[u"); // restore cursor
                fflush(stdout);
                
                (*loop)++;
            }
        }
    // }
    // }
    // return result ? result : NULL;
}