#pragma once

/*
 * Support for ISO/IEC 14496-3:2001(E) AAC ADTS audio streams.
 *
 */

#include "audio_defns.h"
// AAC ADTS provides audio in frames of constant time

// Flags for ``read_next_adts_frame``
//
// Specify this flag to indicate that there is no emphasis field in the ADTS
// header. Generally, MPEG-2 ADTS audio (ID=0) has no emphasis field and
// MPEG-4 (ID=1) has emphasis, but some H.264/AAC streams have MPEG-4 ADTS
// with no emphasis and in those cases, you'll need this flag.
#define ADTS_FLAG_NO_EMPHASIS (1 << 0)
// Specify this flag to indicate that there is always an emphasis field, even
// if the ID says there isn't one - included for symmetry with NO_EMPHASIS.
#define ADTS_FLAG_FORCE_EMPHASIS (1 << 1)
