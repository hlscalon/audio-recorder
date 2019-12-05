#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <sox/sox.h>

#include "encode.h"

int init_sox() {
    return sox_init() == SOX_SUCCESS;
}

void quit_sox() {
    sox_quit();
}

/**
 * Receive the path of tmp file containing raw format,
 * and path of file which will be used to store mp3
 */
void encode(uint8_t * raw_buf, uint8_t * out_buf, size_t buf_size) {
    static sox_format_t * in, * out; // input and output files, must be static (?)
    sox_effects_chain_t * chain;
    sox_effect_t * e;
    char * args[10];

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

    out = sox_open_mem_write(out_buf, buf_size, &in->signal, NULL, "MP3", NULL);
    assert(out != NULL);

    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    e = sox_create_effect(sox_find_effect("input"));
    args[0] = (char *)in, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);

    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    e = sox_create_effect(sox_find_effect("output"));
    args[0] = (char *)out, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    sox_flow_effects(chain, NULL, NULL);

    sox_delete_effects_chain(chain);

    sox_close(out);
    sox_close(in);
}
