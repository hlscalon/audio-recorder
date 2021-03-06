#ifndef GUI_WIN_H
#define GUI_WIN_H

#include <gtk/gtk.h>
#include "gui.h"

#define AUDIO_RECORDER_GUI_WINDOW_TYPE (audio_recorder_gui_window_get_type())
G_DECLARE_FINAL_TYPE(AudioRecorderGuiWindow, audio_recorder_gui_window, AUDIO_RECORDER, GUI_WINDOW, GtkApplicationWindow)

AudioRecorderGuiWindow *audio_recorder_gui_window_new(AudioRecorderGui *app, gint n_files, GFile **files);
void audio_recorder_gui_window_open(AudioRecorderGuiWindow *win, GFile *file);

typedef struct {
    gchar *filename;
    int stop;
    GMutex mutex_stop;
} SharedRecordData;

typedef struct {
    SharedRecordData * s_record_data;
    gint count;
    gint can_free;
} WorkerData;

#endif
