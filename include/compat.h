#pragma once

/*
 * Standard names for various quantities.
 *
 * These are:
 *
 * 1. for historical reasons
 * 2. to handle differences between BSD or Linux and Windows
 *
 */

#include <cmath>
#include <cstdint>
#include <string>

// Keep "byte" for historical/affectionate reasons
typedef uint8_t byte;

#include <cinttypes>
#include <sys/types.h>
#include <unistd.h>
typedef off_t offset_t;

// If Linux does not have 64 bit support built in, then our offsets will
// be just 32 bit integers
#define OFFSET_T_FORMAT "%ld"
#define OFFSET_T_FORMAT_08 "%08ld" // deprecated, because it looks like hex/octal
#define OFFSET_T_FORMAT_8 "%8ld"

// Whilst we're at it, define the format for a 64 bit integer as such
#define LLD_FORMAT "%" PRId64
#define LLU_FORMAT "%" PRIu64
#define LLD_FORMAT_STUMP "lld"
#define LLU_FORMAT_STUMP "llu"

// Other useful things

typedef void* void_p;

// The following defaults are common, and it's difficult
// to decide which other header file they might belong in
#define DEFAULT_VIDEO_PID 0x68
#define DEFAULT_AUDIO_PID 0x67
#define DEFAULT_PMT_PID 0x66
