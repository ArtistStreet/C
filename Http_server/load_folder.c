#include "lib.h"

typedef struct {
    char *file_name;
    char *content;
} FileEntry;

FileEntry *load_file(const char *folder_path, int *count) {
    DIR *dir = opendir(folder_path); // Open the directory
    struct dirent *entry; // Directory entry
    FileEntry *files = NULL; // Array of FileEntry
    *count = 0; // Number of files

    if (!dir) {
        perror("Failed to open directory");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) { // Regular file
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", folder_path, entry->d_name); // Create full path

            FILE *file = fopen(path, "r"); // Open the file
            if (!file) {
                perror("Failed to open file");
                continue;
            }

            fseek(file, 0, SEEK_END); // Move to the end of the file
            long size = ftell(file); // Get the size of the file
            fseek(file, 0, SEEK_SET); // Move back to the beginning

            char *data = malloc(size + 1); // Allocate memory for file content
            if (!data) {
                perror("Failed to allocate memory");
                fclose(file);
                continue;
            }

            fread(data, 1, size, file); // Read the file content
            data[size] = '\0'; // Null-terminate the string
            fclose(file);

            files = realloc(files, (*count + 1) * sizeof(FileEntry)); // Resize the array
            if (!files) {
                perror("Failed to allocate memory for files");
                free(data);
                continue;
            }
            files[*count].file_name = strdup(entry->d_name); // Duplicate the file name
            files[*count].content = data; // Store the file content
            (*count)++; // Increment the file count
        } 
    }

    closedir(dir);
    return files;
}