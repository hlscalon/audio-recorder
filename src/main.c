#include <stdio.h>
#include <gtk/gtk.h>

#include "gui.h"

int main(int argc, char * argv[]) {
    return g_application_run(G_APPLICATION(audio_recorder_gui_new()), argc, argv);
}
