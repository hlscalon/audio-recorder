// free(), malloc()
#include <stdlib.h>

#include "audio_file.h"

/**
 * Open file or create one, if it does not exist
 */
int open_files(audio_file * file) {
    if ((file->f = fopen(file->name, "w+")) == NULL) {
        return 0;
    }

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

    if (file->f) {
        fclose(file->f);
    }

    if (file->f_tmp) {
        fclose(file->f_tmp);
    }

    free(file);
}
