#pragma once

/*
 * Support for ISO/IEC 14496-3:2001(E) AAC ADTS audio streams.
 *
 */

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "adts_fns.h"
#include "compat.h"
#include "misc_fns.h"
#include "printing_fns.h"

#define DEBUG 0

/*
 * Read the next ADTS frame.
 *
 * Assumes that the input stream is synchronised - i.e., it does not
 * try to cope if the next three bytes are not '1111 1111 1111'.
 *
 * - `file` is the file descriptor of the ADTS file to read from
 * - `frame` is the ADTS frame that is read
 * - `flags` indicates if we are forcing the recognition of "emphasis"
 *   fields, etc.
 *
 * Returns 0 if all goes well, EOF if end-of-file is read, and 1 if something
 * goes wrong.
 */
int read_next_adts_frame(int file, audio_frame_p* frame, unsigned int flags)
{
#define JUST_ENOUGH 6 // just enough to hold the bits of the headers we want

    int err, ii;
    int id, layer;
    byte header[JUST_ENOUGH];
    byte* data = nullptr;
    int frame_length;
    int has_emphasis = 0;

    offset_t posn = tell_file(file);
#if DEBUG
    fprint_msg("Offset: " OFFSET_T_FORMAT "\n", posn);
#endif

    err = read_bytes(file, JUST_ENOUGH, header);
    if (err == EOF)
        return EOF;
    else if (err) {
        fprint_err("### Error reading header bytes of ADTS frame\n"
                   "    (in frame starting at " OFFSET_T_FORMAT ")\n",
            posn);
        free(data);
        return 1;
    }

#if DEBUG
    print_msg("ADTS frame\n");
    print_data(true, "Start", header, JUST_ENOUGH, JUST_ENOUGH);
#endif

    if (header[0] != 0xFF || (header[1] & 0xF0) != 0xF0) {
        fprint_err("### ADTS frame does not start with '1111 1111 1111'"
                   " syncword - lost synchronisation?\n"
                   "    Found 0x%X%X%X instead of 0xFFF\n",
            (unsigned)(header[0] & 0xF0) >> 4, (header[0] & 0x0F),
            (unsigned)(header[1] & 0xF0) >> 4);
        fprint_err("    (in frame starting at " OFFSET_T_FORMAT ")\n", posn);
        return 1;
    }

    id = (header[1] & 0x08) >> 3;
#if DEBUG
    fprint_msg("   ID %d (%s)\n", id, (id == 1 ? "MPEG-2 AAC" : "MPEG-4"));
#endif
    layer = (header[1] & 0x06) >> 1;
    if (layer != 0)
        fprint_msg("   layer is %d, not 0 (in frame at " OFFSET_T_FORMAT ")\n", layer, posn);

    // Experience appears to show that emphasis doesn't exist in MPEG-2 AVC.
    // But it does exist in (ID=1) MPEG-4 streams.
    //
    // .. or if forced.

    has_emphasis
        = (flags & ADTS_FLAG_NO_EMPHASIS) ? 0 : ((flags & ADTS_FLAG_FORCE_EMPHASIS) || !id);

    if (!has_emphasis) {
        frame_length
            = ((header[3] & 0x03) << 11) | (header[4] << 3) | ((unsigned)(header[5] & 0xE0) >> 5);
    } else {
        frame_length = (header[4] << 5) | ((unsigned)(header[5] & 0xF8) >> 3);
    }
#if DEBUG
    fprint_msg("   length %d\n", frame_length);
#endif

    data = (byte*)malloc(frame_length);
    if (data == nullptr) {
        print_err("### Unable to extend data buffer for ADTS frame\n");
        free(data);
        return 1;
    }

    for (ii = 0; ii < JUST_ENOUGH; ii++)
        data[ii] = header[ii];

    err = read_bytes(file, frame_length - JUST_ENOUGH, &(data[JUST_ENOUGH]));
    if (err) {
        if (err == EOF)
            print_err("### Unexpected EOF reading rest of ADTS frame\n");
        else
            print_err("### Error reading rest of ADTS frame\n");
        free(data);
        return 1;
    }
#if DEBUG
    print_data(true, "Again", data, frame_length, 20);
#endif

    err = build_audio_frame(frame);
    if (err) {
        free(data);
        return 1;
    }
    (*frame)->data = data;
    (*frame)->data_len = frame_length;

    return 0;
}
