#pragma once
#include "types.h"
#include <stdbool.h>

typedef struct {
    char *data;
    u32 len;
    bool is_valid;
} File;

File io_file_read(char *path);
