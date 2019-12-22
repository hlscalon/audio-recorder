#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#define AUDIO_RECORDER_GUI_TYPE (audio_recorder_gui_get_type())
G_DECLARE_FINAL_TYPE(AudioRecorderGui, audio_recorder_gui, AUDIO_RECORDER, GUI, GtkApplication)

AudioRecorderGui *audio_recorder_gui_new();

#endif
