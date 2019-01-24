#pragma once

/*
 * Generic support for audio streams.
 *
 */

#include "audio_defns.h"

/*
 * Build a new generic audio frame datastructure
 *
 * Returns 0 if all goes well, 1 if something goes wrong.
 */
int build_audio_frame(audio_frame_p* frame);

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
 * try to cope if (for MPEG2) the next three bytes are not '1111 1111 1111'.
 *
 * - `file` is the file descriptor of the audio file to read from
 * - `audio_type` indicates what type of audio - e.g., AUDIO_ADTS
 * - `frame` is the audio frame that is read
 *
 * Returns 0 if all goes well, EOF if end-of-file is read, and 1 if something
 * goes wrong.
 */
int read_next_audio_frame(int file, int audio_type, audio_frame_p* frame);
