#ifndef TARO_FILE_IO
#define TARO_FILE_IO

#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

typedef struct
{
    char *data;
    size_t size;
} TrFile;

TrFile *taro_read_file(const char *path)
{
    FILE *f     = fopen(path, "r");
    char *buf   = NULL;
    size_t size = 0;

    if (!f) {
        log_error("failed to open file %s\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    buf       = (char *)malloc(size + 1);
    buf[size] = '\0';

    if (!buf) {
        log_error("failed to allocate buffer for file %s\n", path);
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, size, f);
    if (read != size) {
        log_error("failed to read file %s\n", path);
        fclose(f);
        free(buf);
        return NULL;
    }

    TrFile *file = (TrFile *)malloc(sizeof(TrFile));
    file->data   = buf;
    file->size   = size;

    fclose(f);
    return file;
}

void taro_file_free(TrFile *file)
{
    free(file->data);
    free(file);
}

#endif