#include <gtk/gtk.h>

#include "gui.h"
#include "gui_win.h"

struct _AudioRecorderGuiWindow {
    GtkApplicationWindow parent;

    GtkWidget *label_result;

    GFile *file;
};

G_DEFINE_TYPE(AudioRecorderGuiWindow, audio_recorder_gui_window, GTK_TYPE_APPLICATION_WINDOW);

int stop = 0;
GMutex mutex_stop;
GMutex mutex_ui;

typedef struct {
    GtkWidget *label_result;
    int count;
    int can_free;
} WorkerData;

static gboolean update_gui(gpointer data) {
    WorkerData * d = (WorkerData *)data;

    gchar *str_count = g_strdup_printf("%d", d->count);

    g_mutex_lock(&mutex_ui);
    gtk_label_set_text(GTK_LABEL(d->label_result), str_count);
    g_mutex_unlock(&mutex_ui);

    g_free(str_count);

    d->can_free = 1;

    return false;
}

static gpointer record(gpointer data) {
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

    g_free(d);

    return NULL;
}

static void btn_start_record_clicked(GtkButton *button, AudioRecorderGuiWindow * win) {
    g_print("Recordddddddddddd\n");

    // ret = record_audio(argv[1]);

    WorkerData * d = g_malloc(sizeof *d);
    d->label_result = win->label_result;
    d->count = 0;
    d->can_free = 0;

    GThread *thread = g_thread_new("record", record, d);
    g_thread_unref(thread);
}

static void btn_stop_record_clicked(GtkButton *button, AudioRecorderGuiWindow * win) {
    g_print("Stopppppppp\n");

    g_mutex_lock(&mutex_stop);
    stop = 1;
    g_mutex_unlock(&mutex_stop);

    g_print("stop = %d\n", stop);
}

static void audio_recorder_gui_window_init(AudioRecorderGuiWindow *win) {
    gtk_widget_init_template(GTK_WIDGET(win));
}

static void audio_recorder_gui_window_class_init(AudioRecorderGuiWindowClass *class) {
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class), "/org/gtk/gui/window.ui");

    // gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), AudioRecorderGuiWindow, btn_record);
    gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), AudioRecorderGuiWindow, label_result);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), btn_start_record_clicked);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), btn_stop_record_clicked);
}

AudioRecorderGuiWindow * audio_recorder_gui_window_new(AudioRecorderGui *app, gint n_files, GFile ** files) {
    AudioRecorderGuiWindow * win = g_object_new(AUDIO_RECORDER_GUI_WINDOW_TYPE, "application", app, NULL);
    win->file = files[0];

    return win;
}

void audio_recorder_gui_window_open(AudioRecorderGuiWindow *win, GFile *file) {}
