#include "../lib.h"

void daemonize() {
    pid_t pid = fork();

    if (pid < 0) exit(EXIT_FAILURE); // fork lỗi
    if (pid > 0) exit(EXIT_SUCCESS); // thoát tiến trình cha

    setsid(); // tạo session mới
    umask(0); // bỏ hạn chế quyền
    chdir("/"); // chuyển về root directory

    // Đóng tất cả descriptor
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}