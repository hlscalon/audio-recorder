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

void encode(char * in_file, char * out_file) {
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
    assert((in = sox_open_read(in_file, &info, &encoding, "RAW")));

    // open output buffer
    struct sox_encodinginfo_t encoding_out;
    // encoding_out.encoding = SOX_ENCODING_MP3;
    encoding_out.encoding = SOX_ENCODING_SIGN2;
    // encoding_out.bits_per_sample = 0;
    encoding_out.bits_per_sample = 16;
    // encoding_out.compression = 32; // bitrate
    encoding_out.compression = 0; // bitrate
    encoding_out.reverse_bytes = sox_option_default;
    encoding_out.reverse_nibbles = sox_option_default;
    encoding_out.reverse_bits = sox_option_default;
    encoding_out.opposite_endian = sox_false;

    struct sox_signalinfo_t signal_out;
    // signal_out.rate = 16000; // 128 ?
    // signal_out.channels = 1;
    signal_out.rate = 44100; // 128 ?
    signal_out.channels = 2;
    signal_out.precision = 0;
    signal_out.length = 0;
    signal_out.mult = NULL;

    sox_format_t * out;
    assert((out = sox_open_write(out_file, &signal_out, &encoding_out, "MP3", NULL, NULL)));

    sox_effects_chain_t * chain;
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    char * args[10];
    sox_effect_t * e;

    sox_signalinfo_t interm_signal = in->signal;

    e = sox_create_effect(sox_find_effect("input"));
    args[0] = (char *)in;
    assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &interm_signal, &in->signal) == SOX_SUCCESS);
    free(e);

    e = sox_create_effect(sox_find_effect("rate"));
    assert(sox_effect_options(e, 0, NULL) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &interm_signal, &out->signal) == SOX_SUCCESS);
    free(e);

    e = sox_create_effect(sox_find_effect("channels"));
    assert(sox_effect_options(e, 0, NULL) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &interm_signal, &out->signal) == SOX_SUCCESS);
    free(e);

    e = sox_create_effect(sox_find_effect("output"));
    args[0] = (char *)out;
    assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    // assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &interm_signal, &out->signal) == SOX_SUCCESS);
    free(e);

    sox_flow_effects(chain, NULL, NULL);
    sox_delete_effects_chain(chain);

    sox_close(out);
    sox_close(in);
}
