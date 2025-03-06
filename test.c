#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>

void enableRawMode(struct termios *old) {
    struct termios new;
    tcgetattr(STDIN_FILENO, old);
    new = *old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void disableRawMode(struct termios *old) {
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}

// void listDirectories(const char *path) {
//     DIR *d = opendir(path);
//     struct dirent *dir;
//     if (d) {
//         printf("\nCác thư mục có sẵn:\n");
//         while ((dir = readdir(d)) != NULL) {
//             if (dir->d_type == DT_DIR && dir->d_name[0] != '.') {
//                 printf("%s  ", dir->d_name);
//             }
//         }
//         printf("\nNhập tiếp: ");
//         closedir(d);
//     } else {
//         printf("\nKhông thể mở thư mục.\n");
//     }
// }


int main() {
    struct termios oldt;
    enableRawMode(&oldt);

    char buffer[100] = {0};
    int index = 0;

    printf("Nhập lệnh (nhấn 'q' để thoát): ");

    while (1) {
        char c = getchar();
        
        if (c == 'q') {
            break;
        }

        // Xử lý phím Tab sau "cd "
        if (index >= 3 && strncmp(buffer, "cd ", 3) == 0 && c == '\t') {
            // listDirectories(".");  // Hiển thị thư mục con
            printf("1");
        } else {
            buffer[index++] = c;
            buffer[index] = '\0';  // Kết thúc chuỗi

            if (c == '\n') {
                printf("Lệnh đã nhập: %s", buffer);
                memset(buffer, 0, sizeof(buffer));
                index = 0;
                printf("\nNhập lệnh: ");
            }
        }
    }

    disableRawMode(&oldt);
    return 0;
}
