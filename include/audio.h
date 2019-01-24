#pragma once

/*
 * Generic audio functionality
 *
 */

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "ac3_fns.h"
#include "adts_fns.h"
#include "audio_fns.h"
#include "compat.h"
#include "l2audio_fns.h"
#include "printing.h"

/*
 * Build a new generic audio frame datastructure
 *
 * Returns 0 if all goes well, 1 if something goes wrong.
 */
inline int build_audio_frame(audio_frame_p* frame)
{
    audio_frame_p new2 = (audio_frame_p)malloc(SIZEOF_AUDIO_FRAME);
    if (new2 == nullptr) {
        fprint_err("### Unable to allocate audio frame datastructure\n");
        return 1;
    }

    new2->data = nullptr;
    new2->data_len = 0;

    *frame = new2;
    return 0;
}

/*
 * Tidy up and free an audio frame datastructure when we've finished with it
 *
 * Empties the datastructure, frees it, and sets `frame` to nullptr.
 *
 * If `frame` is already nullptr, does nothing.
 */
void free_audio_frame(audio_frame_p* frame)
{
    if (*frame == nullptr)
        return;

    if ((*frame)->data != nullptr) {
        free((*frame)->data);
        (*frame)->data = nullptr;
    }
    (*frame)->data_len = 0;

    free(*frame);
    *frame = nullptr;
}
/*
 * Read the next audio frame.
 *
 * Assumes that the input stream is synchronised - i.e., it does not
 * try to cope if (for MPEG2) the next three bytes are not '1111 1111 1111'.
 *
 * - `file` is the file descriptor of the audio file to read from
 * - `audio_type` indicates what type of audio - e.g., AUDIO_ADTS
 * - `frame` is the audio frame that is read
 *
 * Returns 0 if all goes well, EOF if end-of-file is read, and 1 if something
 * goes wrong.
 */
int read_next_audio_frame(int file, int audio_type, audio_frame_p* frame)
{
    switch (audio_type) {
    case AUDIO_ADTS_MPEG2:
        return read_next_adts_frame(file, frame, ADTS_FLAG_NO_EMPHASIS);
    case AUDIO_ADTS_MPEG4:
        return read_next_adts_frame(file, frame, ADTS_FLAG_FORCE_EMPHASIS);
    case AUDIO_ADTS:
        return read_next_adts_frame(file, frame, 0);
    case AUDIO_L2:
        return read_next_l2audio_frame(file, frame);
    case AUDIO_AC3:
        return read_next_ac3_frame(file, frame);
    default:
        fprint_err("### Unrecognised audio type %d - cannot get next audio frame\n", audio_type);
        return 1;
    }
}
