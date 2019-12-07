#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>
#include <stdlib.h>
#include <sox/sox.h>

int init_sox();
void quit_sox();
void encode(uint8_t * raw_buf, size_t buf_size, sox_format_t * out);
sox_format_t * open_file(const char * file_name);
void close_file(sox_format_t * out);

#endif
