#pragma once

/*
 * Support for ATSC Digital Audio Compression Standard, Revision B
 * (AC3) audio streams.
 *
 */

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "ac3_fns.h"
#include "compat.h"
#include "misc_fns.h"
#include "printing_fns.h"

static const unsigned int // Table 5.18, frame sizes
    l_frmsizecod[19][3]
    = { { 64, 69, 96 }, { 80, 87, 120 }, { 96, 104, 144 }, { 112, 121, 168 }, { 128, 139, 192 },
          { 160, 174, 240 }, { 192, 208, 288 }, { 224, 243, 336 }, { 256, 278, 384 },
          { 320, 348, 480 }, { 384, 417, 576 }, { 448, 487, 672 }, { 512, 557, 768 },
          { 640, 696, 960 }, { 768, 835, 1152 }, { 896, 975, 1344 }, { 1024, 1114, 1536 },
          { 1152, 1253, 1728 }, { 1280, 1393, 1920 } };

/*
 * Read the next AC3 frame.
 *
 * Assumes that the input stream is synchronised - i.e., it does not
 * try to cope if the next two bytes are not '0000 1011 0111 0111'
 *
 * - `file` is the file descriptor of the AC3 file to read from
 * - `frame` is the AC3 frame that is read
 *
 * Returns 0 if all goes well, EOF if end-of-file is read, and 1 if something
 * goes wrong.
 */
#define SYNCINFO_SIZE 5

int read_next_ac3_frame(int file, audio_frame_p* frame)
{
    int i, err;
    byte sync_info[SYNCINFO_SIZE];
    byte* data = nullptr;
    int fscod;
    int frmsizecod;
    int frame_length;

    offset_t posn = tell_file(file);

    err = read_bytes(file, SYNCINFO_SIZE, sync_info);
    if (err == EOF)
        return EOF;
    else if (err) {
        fprint_err("### Error reading syncinfo from AC3 file\n"
                   "    (in frame starting at " OFFSET_T_FORMAT ")\n",
            posn);
        return 1;
    }

    if (sync_info[0] != 0x0b || sync_info[1] != 0x77) {
        fprint_err("### AC3 frame does not start with 0x0b77"
                   " syncword - lost synchronisation?\n"
                   "    Found 0x%02x%02x instead of 0x0b77\n",
            (unsigned)sync_info[0], (unsigned)sync_info[1]);
        fprint_err("    (in frame starting at " OFFSET_T_FORMAT ")\n", posn);
        return 1;
    }

    fscod = sync_info[4] >> 6;
    if (fscod == 3) {
        // Bad sample rate code
        fprint_err("### Bad sample rate code in AC3 syncinfo\n"
                   "    (in frame starting at " OFFSET_T_FORMAT ")\n",
            posn);
        return 1;
    }

    frmsizecod = sync_info[4] & 0x3f;
    if (frmsizecod > 37) {
        fprint_err("### Bad frame size code %d in AC3 syncinfo\n", frmsizecod);
        fprint_err("    (in frame starting at " OFFSET_T_FORMAT ")\n", posn);
        return 1;
    }

    frame_length = l_frmsizecod[frmsizecod >> 1][fscod];
    if (fscod == 1)
        frame_length += frmsizecod & 1;
    frame_length <<= 1; // Convert from 16-bit words to bytes

    data = (byte*)malloc(frame_length);
    if (data == nullptr) {
        print_err("### Unable to extend data buffer for AC3 frame\n");
        return 1;
    }
    for (i = 0; i < SYNCINFO_SIZE; i++)
        data[i] = sync_info[i];

    err = read_bytes(file, frame_length - SYNCINFO_SIZE, &data[SYNCINFO_SIZE]);
    if (err) {
        if (err == EOF)
            print_err("### Unexpected EOF reading rest of AC3 frame\n");
        else
            print_err("### Error reading rest of AC3 frame\n");
        free(data);
        return 1;
    }

    err = build_audio_frame(frame);
    if (err) {
        free(data);
        return 1;
    }
    (*frame)->data = data;
    (*frame)->data_len = frame_length;

    return 0;
}
