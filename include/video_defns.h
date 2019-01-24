#pragma once
/*
 * Support for generic video streams
 *
 */

#include "h222_defns.h"

// Recognised types of video input
// These are convenience names, defined in terms of the H222 values
#define VIDEO_UNKNOWN 0 // which is a reserved value
#define VIDEO_H262 MPEG2_VIDEO_STREAM_TYPE
#define VIDEO_H264 AVC_VIDEO_STREAM_TYPE
#define VIDEO_AVS AVS_VIDEO_STREAM_TYPE
#define VIDEO_MPEG4_PART2 MPEG4_PART2_VIDEO_STREAM_TYPE
