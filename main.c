#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define BUFSIZE 1024

/**
 * Write data to files (raw)
 */
static ssize_t loop_write_file(FILE * file, const void * data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;

        if ((r = fwrite(data, sizeof data[0], size, file)) < 0) {
            return r;
        }

        if (r == 0)
            break;

        ret += r;
        data = (const uint8_t*) data + r;
        size -= (size_t) r;
    }

    return ret;
}

/**
 * Open file or create one, if it does not exist
 */
FILE * open_file(char * filename) {
    FILE * f;

    if ((f = fopen(filename, "w+"))) {
    // if ((f = fopen(filename, "ab+"))) {
        return f;
    }

    return NULL;
}

void print_help() {
    printf("./audio-recorder <file>\n");
}

int record_audio(FILE * file) {
    /**
     * Signed 16 bits, Little Endian
     * 44.1kb
     * Dual channel
     */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };

    pa_simple * s = NULL;
    int error;
    int ret = 1;

    // Create the recording stream
    const char device[] = "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor";
    if (!(s = pa_simple_new(NULL, "audio-recorder", PA_STREAM_RECORD, device, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish2;
    }

    for (int i = 0;; i++) {
        uint8_t buf[BUFSIZE];

        // Record data
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish2;
        }

        // Write to file and check if all data was written correctly
        if (loop_write_file(file, buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish2;
        }
    }

    ret = 0;

finish2:
    if (s) {
        pa_simple_free(s);
    }

    return ret;
}

int main(int argc, char * argv[]) {
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

    return ret;
}
