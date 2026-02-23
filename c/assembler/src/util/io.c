#include "io.h"
#include <stdio.h>
#include <stdlib.h>

// 20 mib, can probably change this to a higher value without issue
#define IO_READ_CHUNK_SIZE 2097152
#define IO_READ_ERROR_GENERAL "Error reading file: %s.errno: %s\n"
#define IO_READ_ERROR_MEMORY "Not enough free memory to read file: %s\n"

File io_file_read(char *path) {
    File file = { .is_valid = false };
    FILE *fp = fopen(path, "rb");
    
    // Check if file opened successfully
    if (!fp) {
        printf("Failed to open file: %s\n", path);
        return file;
    }
    
    if (ferror(fp)) {
        // ERROR_RETURN(file, IO_READ_ERROR_GENERAL, path, errno);
        printf("Fix the io module\n");
        fclose(fp);
        return file;
    }
    
    char *data = NULL;
    char *tmp;
    int used = 0;
    int size = 0;
    int n;
    while (true) {
        if (used + IO_READ_CHUNK_SIZE + 1 > size) {
            size = used + IO_READ_CHUNK_SIZE + 1;
            if (size <= used) {
                free(data);
                // ERROR_RETURN(file, "Input file too large: %s\n", path);
                printf("Fix the io module\n");
                fclose(fp);
                return file;
            }
            tmp = realloc(data, size);
            if (!tmp) {
                free(data);
                // ERROR_RETURN(file, IO_READ_ERROR_MEMORY, path);
                printf("Fix the io module\n");
                fclose(fp);
                return file;
            }
            data = tmp;
        }
        n = fread(data + used, 1, IO_READ_CHUNK_SIZE, fp);
        if (n == 0)
            break;
        used += n;
    }
    if (ferror(fp)) {
        free(data);
        // ERROR_RETURN(file, IO_READ_ERROR_GENERAL, path, errno);
        printf("Fix the io module\n");
        fclose(fp);
        return file;
    }
    tmp = realloc(data, used + 1);
    if (!tmp) {
        free(data);
        // ERROR_RETURN(file, IO_READ_ERROR_MEMORY, path);
        printf("Fix the io module\n");
        fclose(fp);
        return file;
    }
    data = tmp;
    data[used] = 0;
    file.data = data;
    file.len = used;
    file.is_valid = true;
    fclose(fp);
    return file;
}
