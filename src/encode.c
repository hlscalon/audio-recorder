#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include <assert.h>
#include <stdint.h>
#include <sox/sox.h>

#include "encode.h"

int init_sox() {
    return sox_init() == SOX_SUCCESS;
}

void quit_sox() {
    sox_quit();
}

/**
 * Open write file
 */
sox_format_t * open_file(const char * file_name) {
    struct sox_signalinfo_t info = {
        .rate = 44100,
        .channels = 2,
        .precision = 16,
        .length = 0,
        .mult = NULL,
    };

    static sox_format_t * out;
    out = sox_open_write(file_name, &info, NULL, NULL, NULL, NULL);
    assert(out != NULL);

    return out;
}

void close_file(sox_format_t * out) {
    sox_close(out);
}

/**
 * Receive the path of tmp file containing raw format,
 * and path of file which will be used to store mp3
 */
void encode(uint8_t * raw_buf, size_t buf_size, sox_format_t * out) {
    static sox_format_t * in; // input and output files, must be static (?)

    struct sox_signalinfo_t info;
    info.rate = 44100;
    info.channels = 2;
    info.precision = 16;
    info.length = 0;
    info.mult = NULL;

    struct sox_encodinginfo_t encoding;
    encoding.encoding = SOX_ENCODING_SIGN2;
    encoding.bits_per_sample = 16;
    encoding.compression = 0;
    encoding.reverse_bytes = sox_option_default;
    encoding.reverse_nibbles = sox_option_default;
    encoding.reverse_bits = sox_option_default;
    encoding.opposite_endian = sox_false;

    // #TODO: remove assert's
    in = sox_open_mem_read(raw_buf, buf_size, &info, &encoding, "RAW");
    assert(in != NULL);

    #define MAX_SAMPLES (size_t)2048
    sox_sample_t samples[MAX_SAMPLES];

    size_t number_read;
    while ((number_read = sox_read(in, samples, MAX_SAMPLES))) {
        assert(sox_write(out, samples, number_read) == number_read);
    }

    sox_close(in);
}
