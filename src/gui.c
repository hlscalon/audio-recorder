#include <gtk/gtk.h>

#include "gui.h"
#include "gui_win.h"
#include "record.h"
#include "encode.h"

struct _AudioRecorderGui {
    GtkApplication parent;
};

G_DEFINE_TYPE(AudioRecorderGui, audio_recorder_gui, GTK_TYPE_APPLICATION);

static void audio_recorder_gui_init(AudioRecorderGui *app) {}

static void print_help() {
    printf("./audio-recorder <file>\n");
}

static void audio_recorder_gui_open(GApplication *app, GFile **files, gint n_files, const gchar *hint) {
    // if (init_sox() == 0) {
    //     fprintf(stderr, __FILE__": init_sox() failed\n");
    //     return 1;
    // }

    // g_print("argv[0] = %s\n", g_file_get_basename(argv[0]));

    if (n_files < 1) {
        g_printerr(__FILE__": filename must be informed\n");
        print_help();

        g_application_quit(app);
        return;
    }

    AudioRecorderGuiWindow *win;

    win = audio_recorder_gui_window_new(AUDIO_RECORDER_GUI(app), n_files, files);

    gtk_window_present(GTK_WINDOW(win));

// finish:
//     quit_sox();

}

static void audio_recorder_gui_activate(GApplication *app) {}

static void audio_recorder_gui_class_init(AudioRecorderGuiClass *class) {
    G_APPLICATION_CLASS(class)->activate = audio_recorder_gui_activate;
    G_APPLICATION_CLASS(class)->open = audio_recorder_gui_open;
}

AudioRecorderGui * audio_recorder_gui_new() {
    return g_object_new(AUDIO_RECORDER_GUI_TYPE, "application-id", "org.gtk.audio_recorder_gui", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
