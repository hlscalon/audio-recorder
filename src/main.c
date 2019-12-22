#include <stdio.h>
#include <gtk/gtk.h>

#include "record.h"
#include "encode.h"
#include "gui.h"

static void print_help() {
    printf("./audio-recorder <file>\n");
}

int main(int argc, char * argv[]) {
    if (init_sox() == 0) {
        fprintf(stderr, __FILE__": init_sox() failed\n");
        return 1;
    }

    int ret = 1;

    if (argc < 2) {
        fprintf(stderr, __FILE__": filename must be informed\n");
        print_help();

        goto finish;
    }

    // ret = record_audio(argv[1]);

    ret = g_application_run(G_APPLICATION(audio_recorder_gui_new()), 0, NULL);

finish:
    quit_sox();

    return ret;
}
