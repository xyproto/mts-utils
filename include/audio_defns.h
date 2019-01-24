#pragma once

/*
 * Support for generic audio streams
 *
 */

#include <cctype>
#include <cmath>
#include <cstdint>
#include <string>

#include "h222_defns.h"

#define byte uint8_t

// A simple wrapper for a frame of audio data
struct audio_frame {
    byte* data; // The frame data, including the syncword at the start
    uint32_t data_len;
};
typedef struct audio_frame* audio_frame_p;
#define SIZEOF_AUDIO_FRAME sizeof(struct audio_frame)

// The types of audio we know about
// These are convenience names, defined in terms of the H222 values
#define AUDIO_UNKNOWN 0 // which is a reserved value
#define AUDIO_ADTS ADTS_AUDIO_STREAM_TYPE
#define AUDIO_L2 MPEG2_AUDIO_STREAM_TYPE
#define AUDIO_AC3 ATSC_DOLBY_AUDIO_STREAM_TYPE

#define AUDIO_ADTS_MPEG2 0x100
#define AUDIO_ADTS_MPEG4 0x101

#define AUDIO_STR(x)                                                                              \
    ((x) == AUDIO_UNKNOWN ? "unknown"                                                             \
                          : (x) == AUDIO_ADTS ? "ADTS"                                            \
                                              : (x) == AUDIO_ADTS_MPEG2 ? "ADTS-MPEG2"            \
                                                                        : (x) == AUDIO_ADTS_MPEG4 \
                        ? "ADTS-MPEG4"                                                            \
                        : (x) == AUDIO_L2 ? "MPEG2" : (x) == AUDIO_AC3 ? "ATSC-AC3" : "???")
