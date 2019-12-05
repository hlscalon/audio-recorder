#ifndef ENCODE_H
#define ENCODE_H

#include <stdint.h>
#include <stdlib.h>

int init_sox();
void quit_sox();
void encode(uint8_t * raw_buf, uint8_t * out_buf, size_t buf_size);

#endif
