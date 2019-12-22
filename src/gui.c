#include <gtk/gtk.h>

#include "gui.h"
#include "gui_win.h"

struct _AudioRecorderGui {
    GtkApplication parent;
};

G_DEFINE_TYPE(AudioRecorderGui, audio_recorder_gui, GTK_TYPE_APPLICATION);

static void audio_recorder_gui_init(AudioRecorderGui *app) {}

static void audio_recorder_gui_activate(GApplication *app) {
    AudioRecorderGuiWindow *win;

    win = audio_recorder_gui_window_new(AUDIO_RECORDER_GUI(app));

    gtk_window_present(GTK_WINDOW(win));
}

static void audio_recorder_gui_class_init(AudioRecorderGuiClass *class) {
    G_APPLICATION_CLASS(class)->activate = audio_recorder_gui_activate;
}

AudioRecorderGui * audio_recorder_gui_new() {
    return g_object_new(AUDIO_RECORDER_GUI_TYPE, "application-id", "org.gtk.audio_recorder_gui", "flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
