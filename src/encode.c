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

void encode(char * in_file, char * out_file) {
    static sox_format_t * in, * out; // input and output files, must be static (?)
    sox_effects_chain_t * chain;
    sox_effect_t * e;
    char * args[10];

    struct sox_signalinfo_t info;

    info.channels = 2;
    info.length = 0;
    info.precision = 16;
    info.rate = 44100;

    struct sox_encodinginfo_t encoding;

    encoding.encoding = SOX_ENCODING_SIGN2;
    encoding.bits_per_sample = 16;
    encoding.opposite_endian = sox_false;

    assert((in = sox_open_read(in_file, &info, &encoding, "RAW")));

    assert((out = sox_open_write(out_file, &in->signal, NULL, NULL, NULL, NULL)));

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
