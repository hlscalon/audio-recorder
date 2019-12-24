#include <gtk/gtk.h>
#include <stdio.h>

#include "gui.h"
#include "gui_win.h"
#include "record.h"

struct _AudioRecorderGuiWindow {
    GtkApplicationWindow parent;

    GtkWidget *label_result;

    SharedRecordData *s_record_data; // #TODO: free this resource
};

G_DEFINE_TYPE(AudioRecorderGuiWindow, audio_recorder_gui_window, GTK_TYPE_APPLICATION_WINDOW);

GMutex mutex_ui;

gboolean update_gui(gpointer data) {
    WorkerData * d = (WorkerData *)data;

    gchar *str_count = g_strdup_printf("%d", d->count);

    // g_mutex_lock(&mutex_ui);
    // gtk_label_set_text(GTK_LABEL(d->win->label_result), str_count);
    // g_mutex_unlock(&mutex_ui);

    g_free(str_count);

    d->can_free = 1;

    return false;
}

/*static gpointer record(gpointer data) {
    g_print("Recording...\n");

    WorkerData * d = (WorkerData *) data;
    stop = 0;

    for (;;) {
        g_mutex_lock(&mutex_stop);
        if (stop) {
            g_mutex_unlock(&mutex_stop);
            break;
        }

        g_mutex_unlock(&mutex_stop);

        d->count++;

        if (d->count % 30000 == 0) {
            d->can_free = 0;
            g_idle_add(update_gui, d);
        }
    }

    g_print("Recording finished\n");
    g_print("i = %d\n", d->count);

    // wait for free signal
    while (!d->can_free) {}

    g_free(d->filename);
    g_free(d);

    return NULL;
}*/

static void btn_start_record_clicked(GtkButton *button, AudioRecorderGuiWindow * win) {
    g_print("btn-start-record\n");

    WorkerData * d = g_malloc(sizeof *d);
    d->s_record_data = win->s_record_data;
    d->count = 0;
    d->can_free = 0;

    GThread *thread = g_thread_new("record_audio", record_audio, d);
    g_thread_unref(thread);
}

static void btn_stop_record_clicked(GtkButton *button, AudioRecorderGuiWindow * win) {
    g_print("btn-stop-record\n");

    g_mutex_lock(&win->s_record_data->mutex_stop);
    win->s_record_data->stop = 1;
    g_mutex_unlock(&win->s_record_data->mutex_stop);

    g_print("stop = %d\n", win->s_record_data->stop);
}

static void audio_recorder_gui_window_init(AudioRecorderGuiWindow *win) {
    gtk_widget_init_template(GTK_WIDGET(win));
}

static void audio_recorder_gui_window_class_init(AudioRecorderGuiWindowClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/gtk/gui/window.ui");

    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), AudioRecorderGuiWindow, label_result);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), btn_start_record_clicked);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), btn_stop_record_clicked);
}

AudioRecorderGuiWindow * audio_recorder_gui_window_new(AudioRecorderGui *app, gint n_files, GFile ** files) {
    AudioRecorderGuiWindow * win = g_object_new(AUDIO_RECORDER_GUI_WINDOW_TYPE, "application", app, NULL);

    win->s_record_data = g_malloc(sizeof *win->s_record_data);
    win->s_record_data->filename = g_file_get_basename(files[0]);
    // g_free(win->s_record_data->filename); // #TODO: free when it is not needed anymore
    win->s_record_data->stop = 0;
    g_mutex_init(&win->s_record_data->mutex_stop);
    // g_mutex_clear(&win->s_record_data->mutex_stop); // #TODO: free when it is not needed anymore

    return win;
}

void audio_recorder_gui_window_open(AudioRecorderGuiWindow *win, GFile *file) {}
