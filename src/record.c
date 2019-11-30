#include <stdio.h>

// For NULL
#include <unistd.h>

// For strerror()
#include <string.h>

// For errno
#include <errno.h>

#include <pulse/simple.h>
#include <pulse/error.h>

#include "record.h"
#include "encode.h"
#include "audio_file.h"

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

int record_audio(audio_file * file) {
    /**
     * Samplerate: 44100Hz
     * Dual channel
     * Signed 16 bits, Little Endian
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
    // #TODO: get this device with other methods
    const char device[] = "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor";
    if (!(s = pa_simple_new(NULL, "audio-recorder", PA_STREAM_RECORD, device, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    printf("Recording...\n");
    for (int i = 0; i < 2000; i++) {
        uint8_t buf[BUFSIZE];

        // Record data
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }

        // write raw pcm to tmp file
        if (loop_write_file(file->f_tmp, buf, sizeof(buf)) != sizeof(buf)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }
    }

    printf("Encoding...\n");

    // close tmp file
    close_file(file->f_tmp);
    encode(file->name_tmp, file->name);

    remove_file(file->name_tmp);

    ret = 0;

finish:
    if (s) {
        pa_simple_free(s);
    }

    return ret;
}
