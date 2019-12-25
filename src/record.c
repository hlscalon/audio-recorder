#include <gtk/gtk.h>
#include <stdio.h>

// For NULL
#include <unistd.h>

// For errno
#include <errno.h>

#include <pulse/simple.h>
#include <pulse/error.h>
#include <sox/sox.h>

#include "record.h"
#include "encode.h"
#include "gui_win.h"

#define BUFSIZE 1024

/**
 * Record raw audio using pulseaudio api,
 * then encoding to mp3
 */
gpointer record_audio(gpointer data) {
    if (init_sox() == 0) {
        g_printerr(__FILE__": init_sox() failed\n");
        return NULL;
    }

    g_print("Recording...\n");

    WorkerData * d = (WorkerData *) data;
    g_mutex_lock(&d->s_record_data->mutex_stop);
    d->s_record_data->stop = 0;
    g_mutex_unlock(&d->s_record_data->mutex_stop);

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
    sox_format_t * out = NULL;

    // Create the recording stream
    // #TODO: get this device with other methods
    const char device[] = "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor";
    if (!(s = pa_simple_new(NULL, "audio-recorder", PA_STREAM_RECORD, device, "record", &ss, NULL, NULL, &error))) {
        g_printerr(__FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }

    out = open_file(d->s_record_data->filename);

    for (;;) {
        g_mutex_lock(&d->s_record_data->mutex_stop);
        if (d->s_record_data->stop) {
            g_mutex_unlock(&d->s_record_data->mutex_stop);
            break;
        }

        g_mutex_unlock(&d->s_record_data->mutex_stop);

        uint8_t buf[BUFSIZE];

        // Record data
        if (pa_simple_read(s, buf, sizeof(buf), &error) < 0) {
            g_printerr(__FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }

        encode(buf, sizeof(buf), out);
        d->count++;
    }

    ret = 0;

finish:
    if (out) {
        close_file(out);
    }

    if (s) {
        pa_simple_free(s);
    }

    quit_sox();

    g_print("Recording finished\n");
    g_print("i = %d\n", d->count);

    // wait for free signal
    // while (!d->can_free) {}

    g_free(d);

    // #TODO: check if error (ret = 1)

    return NULL;
}
