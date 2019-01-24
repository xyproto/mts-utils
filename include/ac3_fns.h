#pragma once

/*
 * Support for ATSC Digital Audio Compression Standard, Revision B
 * (AC3) audio streams.
 *
 */

#include "audio_fns.h"

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
int read_next_ac3_frame(int file, audio_frame_p* frame);
