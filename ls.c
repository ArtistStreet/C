#include "lib.h"

void ls_path(file_system *fs, const char *path) {
    char name[100];
    strcpy(name, path + 2);
    if (name[strlen(path) - 3] == '/') {
        name[strlen(path) - 3] = '\0';
    }

    printf("%s %s %s\n", fs->root->name, path, name);
    node *target = find_node(fs->root, (strcmp(fs->root->name, path) == 0) ? path : name);  // Tìm node theo đường dẫn

    if (!target || !target->isDir) {
        printf("ls: cannot access '%s': No such directory\n", path);
        return;
    }

    node *current = target->child;  // Lấy danh sách con của thư mục

    while (current != NULL) {
        if (current->isDir) {
            printf("\033[1;32m%s\033[0m ", current->name);  // Thư mục (màu xanh)
        } else {
            printf("\033[1;35m%s\033[0m ", current->name);  // Tệp (màu tím)
        }
        current = current->next;
    }

    printf("\n");
}

void ls(file_system *fs, const char *path) {
    if (path == NULL || strcmp(path, "") == 0) {
        ls_path(fs, fs->current->name);  // Liệt kê thư mục hiện tại
    } else {
        if (path[0] == '~') {  // Nếu có dấu `~`, chuyển về thư mục gốc
            char new_path[100];
            snprintf(new_path, sizeof(new_path), "%s%s", fs->root->name, path + 1);
            printf("%s\n", new_path);
            if (strcmp(new_path, "~/") == 0) {  
                ls_path(fs, fs->root->name);
            } else {
                ls_path(fs, new_path);
            }
        } else {
            ls_path(fs, path);
        }
    }
}