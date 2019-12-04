#include <stdio.h>

// For NULL
#include <unistd.h>

// For strerror()
#include <string.h>

// For errno
#include <errno.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <pthread.h>

#include "record.h"
#include "encode.h"
#include "audio_file.h"

#define BUFSIZE 1024

/**
 * Struct to pass to record thread
 */
typedef struct {
    int * stop;
    pa_simple * simple;
    FILE * file_tmp;
} shared_s_record;

/**
 * Mutex to control input stop
 */
pthread_mutex_t mutex;

/**
 * Write data to files (raw)
 */
static ssize_t loop_write_file(FILE * file, const void * data, size_t data_ind_size, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;

        if ((r = fwrite(data, data_ind_size, size, file)) < 0) {
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
 * Thread for recording audio
 * Shared variable (int stop) to finish recording
 */
static void * record_thread(void * s_record_ptr) {
    printf("Recording...\n");

    shared_s_record * r = (shared_s_record *) s_record_ptr;

    int error;
    for (;;) {
        pthread_mutex_lock(&mutex);

        if (*(r->stop)) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);

        uint8_t buf[BUFSIZE];

        // Record data
        if (pa_simple_read(r->simple, buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            return NULL;
        }

        // write raw pcm to tmp file
        if (loop_write_file(r->file_tmp, buf, sizeof(uint8_t), sizeof(buf)) != sizeof(buf)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            return NULL;
        }
    }

    printf("Recording finished\n");

    return NULL;
}

/**
 * Thread for waiting input to finish recording
 */
static void * check_input_thread(void * stop_ptr) {
    int * stop = (int *) stop_ptr;

    int s = 0;

    while (s == 0) {
        printf("\rInput = ");
        scanf("%d", &s);

        if (s == 1) {
            pthread_mutex_lock(&mutex);
            *stop = 1;
            pthread_mutex_unlock(&mutex);

            break;
        }

        if (s != 0) {
            printf("Please input: 0 or 1\n");
        }

        s = 0;
    }

    return NULL;
}

/**
 * Create threads for record and input, returning error|success
 */
static int create_record_threads(pa_simple * s, FILE * tmp) {
    int stop = 0;

    shared_s_record shared_r;
    shared_r.stop = &stop;
    shared_r.simple = s;
    shared_r.file_tmp = tmp;

    // create threads for recording
    pthread_t record_pthread_t, input_pthread_t;

    pthread_mutex_init(&mutex, NULL);

    if (pthread_create(&record_pthread_t, NULL, record_thread, &shared_r)) {
        fprintf(stderr, "Error creating thread\n");
        return 0;
    }

    if (pthread_create(&input_pthread_t, NULL, check_input_thread, &stop)) {
        fprintf(stderr, "Error creating thread\n");
        return 0;
    }

    if (pthread_join(record_pthread_t, NULL)) {
        fprintf(stderr, "Error joining record_pthread_t\n");
        return 0;
    }

    if (pthread_join(input_pthread_t, NULL)) {
        fprintf(stderr, "Error joining input_pthread_t\n");
        return 0;
    }

    return 1;
}

/**
 * Record raw audio using pulseaudio api,
 * then encoding to mp3
 */
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

    if (!create_record_threads(s, file->f_tmp)) {
        goto finish;
    }

    printf("Encoding...\n");

    // close tmp file
    close_file(file->f_tmp);

    // then encode tmp to mp3
    encode(file->name_tmp, file->name);

    // then remove tmp
    remove_file(file->name_tmp);

    ret = 0;

finish:
    if (s) {
        pa_simple_free(s);
    }

    return ret;
}
