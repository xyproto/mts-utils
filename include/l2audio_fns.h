#pragma once

/*
 * Support for MPEG layer 2 audio streams.
 *
 * (actually, support for
 *
 *    - MPEG-1 audio (described in ISO/IEC 11172-3), layers 1..3
 *    - MPEG-2 audio (described in ISO/IEC 13818-3), layer 2
 *    - unofficial MPEG-2.5
 *
 * but MPEG-2 layer 2 is the main target)
 *
 */

#include "audio_defns.h"

/*
 * Tidy up and free an audio frame datastructure when we've finished with it
 *
 * Empties the datastructure, frees it, and sets `frame` to nullptr.
 *
 * If `frame` is already nullptr, does nothing.
 */
void free_audio_frame(audio_frame_p* frame);

/*
 * Read the next audio frame.
 *
 * Assumes that the input stream is synchronised - i.e., it does not
 * try to cope if the next three bytes are not '1111 1111 1111'.
 *
 * - `file` is the file descriptor of the audio file to read from
 * - `frame` is the audio frame that is read
 *
 * Returns 0 if all goes well, EOF if end-of-file is read, and 1 if something
 * goes wrong.
 */
int read_next_l2audio_frame(int file, audio_frame_p* frame);
