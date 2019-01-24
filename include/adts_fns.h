#pragma once

/*
 * Support for ISO/IEC 14496-3:2001(E) AAC ADTS audio streams.
 *
 */

#include "adts_defns.h"
#include "audio_fns.h"

/* Specify this flag to indicate that there is no emphasis
 *  field in the ADTS header. Generally, MPEG-2 ADTS audio
 *  (ID=0) has no emphasis field and MPEG-4 (ID=1) has
 *  emphasis, but some H.264/AAC streams have
 *  MPEG-4 ADTS with no emphasis and in those cases, you'll
 *  need this flag.
 */
#define ADTS_FLAG_NO_EMPHASIS (1 << 0)

/* Specify this flag to indicate that there is always an
 * emphasis field, even if the ID says there isn't one -
 * included for symmetry with NO_EMPHASIS.
 */
#define ADTS_FLAG_FORCE_EMPHASIS (1 << 1)

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
int read_next_adts_frame(int file, audio_frame_p* frame, unsigned int flags);
