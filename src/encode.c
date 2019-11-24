#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "sox/sox.h"
#include "encode.h"

int init_sox() {
    return sox_init() == SOX_SUCCESS;
}

void quit_sox() {
    sox_quit();
}

void encode(uint8_t * in_buffer, uint8_t * out_buffer, size_t buffer_size) {
    struct sox_signalinfo_t info;
    info.channels = 2;
    info.length = 0;
    info.precision = 16;
    info.rate = 44100;

    struct sox_encodinginfo_t encoding;
    encoding.encoding = SOX_ENCODING_SIGN2;
    encoding.bits_per_sample = 16;
    encoding.opposite_endian = sox_false;

    // open raw buffer
    // #TODO: check return
    sox_format_t * in;
    assert((in = sox_open_mem_read(in_buffer, buffer_size, &info, &encoding, "RAW")));

    // open output buffer
    // #TODO: check return
    sox_format_t * out;
    assert((out = sox_open_mem_write(out_buffer, buffer_size, &in->signal, NULL, NULL, NULL)));

    sox_effects_chain_t * chain;
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    char * args[10];
    sox_effect_t * e = sox_create_effect(sox_find_effect("input"));
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
