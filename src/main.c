#include <stdio.h>

// For NULL
#include <unistd.h>

// For strerror(), strcpy()
#include <string.h>

// For errno
#include <errno.h>

// free(), malloc()
#include <stdlib.h>

#include "record.h"
#include "encode.h"
#include "audio_file.h"

static void print_help() {
    printf("./audio-recorder <file>\n");
}

int main(int argc, char * argv[]) {
    if (init_sox() == 0) {
        fprintf(stderr, __FILE__": init_sox() failed\n");
        return 1;
    }

    int ret = 1;
    audio_file * file = malloc(sizeof(audio_file));
    if (file == NULL) {
        fprintf(stderr, __FILE__": error allocating memory\n");
        return 1;
    }

    if (argc < 2) {
        fprintf(stderr, __FILE__": filename must be informed\n");
        print_help();

        goto finish;
    }

    file->name = malloc(strlen(argv[1]) + 1);
    strcpy(file->name, argv[1]);

    file->name_tmp = malloc(strlen(argv[1]) + 4); // for tmp\0
    strcpy(file->name_tmp, argv[1]);
    strcat(file->name_tmp, "tmp");

    if (!open_files(file)) {
        fprintf(stderr, __FILE__": open_files() failed: %s\n", strerror(errno));

        goto finish;
    }

    ret = record_audio(file);

finish:
    free_audio_file(file);

    quit_sox();

    return ret;
}
