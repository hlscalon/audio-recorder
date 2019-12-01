#ifndef AUDIO_FILE_H
#define AUDIO_FILE_H

#include <stdio.h>

typedef struct s_audio_file {
    char * name;
    int length;
    FILE * f;
    char * name_tmp;
    FILE * f_tmp;
} audio_file;

void free_audio_file(audio_file * file);
int open_files(audio_file * file);
void close_file(FILE * file);
int remove_file(const char * path);

#endif
