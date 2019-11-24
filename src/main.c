#include <stdio.h>

// For NULL
#include <unistd.h>

// For strerror()
#include <string.h>

// For errno
#include <errno.h>

#include "record.h"
#include "encode.h"

/**
 * Open file or create one, if it does not exist
 */
FILE * open_file(char * filename) {
    FILE * f;

    if ((f = fopen(filename, "w+"))) {
        return f;
    }

    return NULL;
}

void print_help() {
    printf("./audio-recorder <file>\n");
}

int main(int argc, char * argv[]) {
    if (init_sox() == 0) {
        fprintf(stderr, __FILE__": init_sox() failed\n");
        return 1;
    }

    int ret = 1;
    FILE * file = NULL;

    if (argc < 2) {
        fprintf(stderr, __FILE__": filename must be informed\n");
        print_help();

        goto finish;
    }

    if ((file = open_file(argv[1])) == NULL) {
        fprintf(stderr, __FILE__": open_file() failed: %s\n", strerror(errno));

        goto finish;
    }

    ret = record_audio(file);

finish:
    if (file) {
        fclose(file);
    }

    quit_sox();

    return ret;
}
