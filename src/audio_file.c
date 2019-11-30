// free(), malloc()
#include <stdlib.h>

// remove()
#include <stdio.h>

#include "audio_file.h"

/**
 * Open file or create one, if it does not exist
 */
int open_files(audio_file * file) {
    FILE * f;
    if ((f = fopen(file->name, "w+")) == NULL) {
        return 0;
    }

    close_file(f);

    if ((file->f_tmp = fopen(file->name_tmp, "w+")) == NULL) {
        return 0;
    }

    return 1;
}

void free_audio_file(audio_file * file) {
    if (file->name) {
        free(file->name);
    }

    if (file->name_tmp) {
        free(file->name_tmp);
    }

    free(file);
}

int remove_file(const char * path) {
    return remove(path);
}

void close_file(FILE * file) {
    fclose(file);
}
