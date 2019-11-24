#include "sox/sox.h"

#include "encode.h"

int init_sox() {
    return sox_init() == SOX_SUCCESS;
}

void quit_sox() {
    sox_quit();
}
