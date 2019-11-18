#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include "sox/sox.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/*
 * Reads input file, applies vol & flanger effects, stores in output file.
 * E.g. example1 monkey.au monkey.aiff
 */
int main(int argc, char * argv[])
{
    static sox_format_t * in, * out; /* input and output files */
    sox_effects_chain_t * chain;
    sox_effect_t * e;
    char * args[10];

    assert(argc == 3);

    /* All libSoX applications must start by initialising the SoX library */
    assert(sox_init() == SOX_SUCCESS);

    struct sox_signalinfo_t info;

    info.channels = 2;
    info.length = 0;
    info.precision = 16;
    info.rate = 44100;

    struct sox_encodinginfo_t encoding;

    encoding.encoding = SOX_ENCODING_SIGN2;
    encoding.bits_per_sample = 16;
    encoding.opposite_endian = sox_false;

    /* Open the input file (with default parameters) */
    assert(in = sox_open_read(argv[1], &info, &encoding, "RAW"));

    /* Open the output file; we must specify the output signal characteristics.
     * Since we are using only simple effects, they are the same as the input
     * file characteristics */
    assert(out = sox_open_write(argv[2], &in->signal, NULL, NULL, NULL, NULL));

    /* Create an effects chain; some effects need to know about the input
     * or output file encoding so we provide that information here */
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    /* The first effect in the effect chain must be something that can source
     * samples; in this case, we use the built-in handler that inputs
     * data from an audio file */
    e = sox_create_effect(sox_find_effect("input"));
    args[0] = (char *)in, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    /* This becomes the first `effect' in the chain */
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    /* The last effect in the effect chain must be something that only consumes
     * samples; in this case, we use the built-in handler that outputs
     * data to an audio file */
    e = sox_create_effect(sox_find_effect("output"));
    args[0] = (char *)out, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    /* Flow samples through the effects processing chain until EOF is reached */
    sox_flow_effects(chain, NULL, NULL);

    /* All done; tidy up: */
    sox_delete_effects_chain(chain);

    sox_close(out);
    sox_close(in);
    sox_quit();
    return 0;
}
