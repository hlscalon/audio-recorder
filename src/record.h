#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>
#include <stdint.h>

int record_audio(FILE * file);
void encode(uint8_t * in_buffer, uint8_t * out_buffer, size_t buffer_size);

#endif
