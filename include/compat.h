#pragma once

/*
 * Standard names for various quantities.
 *
 * These are:
 *
 * 1. for historical reasons
 * 2. to handle differences between BSD or Linux and Windows
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the MPEG TS, PS and ES tools.
 *
 * The Initial Developer of the Original Code is Amino Communications Ltd.
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Amino Communications Ltd, Swavesey, Cambridge UK
 *
 * ***** END LICENSE BLOCK *****
 */

// Other than on Windows, using the C99 integer definitions is what people
// expect, so do so
#include <cstdint>

// Keep "byte" for historical/affectionate reasons
typedef uint8_t byte;

// lseek on BSD/Linux uses an off_t quantity to specify the required
// position. Where 64 bit file positions are supported, this is a 64 bit
// value. Unfortunately, Windows has off_t defined as being a long.
// For compatibility, therefore, we define a type that can be used on
// both Windows and Unix
// NB: On some systems, off_t is provided by unistd.h, but on some others
//     it may also be necessary to explicitly include sys/types.h. We shall
//     do both, here, for safety.
#include <cinttypes>
#include <sys/types.h>
#include <unistd.h>
typedef off_t offset_t;

#if defined(__linux__) && !defined(__USE_FILE_OFFSET64)
// If Linux does not have 64 bit support built in, then our offsets will
// be just 32 bit integers
#define OFFSET_T_FORMAT "%ld"
#define OFFSET_T_FORMAT_08 "%08ld" // deprecated, because it looks like hex/octal
#define OFFSET_T_FORMAT_8 "%8ld"
#else
// On Unices, printf supports %lld for 64 bit integers, and this is suitable
// for printing out offset_t when it is 64 bit
#define OFFSET_T_FORMAT "%" PRIi64
#define OFFSET_T_FORMAT_08 "%08" PRIi64 // deprecated, because it looks like hex/octal
#define OFFSET_T_FORMAT_8 "%8" PRIi64
#endif

// Whilst we're at it, define the format for a 64 bit integer as such
#define LLD_FORMAT "%" PRId64
#define LLU_FORMAT "%" PRIu64
#define LLD_FORMAT_STUMP "lld"
#define LLU_FORMAT_STUMP "llu"

// Useful macros, but not side-effect free
#define max(i, j) ((i) > (j) ? (i) : (j))
#define min(i, j) ((i) < (j) ? (i) : (j))

// Other useful things

typedef void* void_p;

#define TRUE 1
#define FALSE 0

// The following defaults are common, and it's difficult
// to decide which other header file they might belong in
#define DEFAULT_VIDEO_PID 0x68
#define DEFAULT_AUDIO_PID 0x67
#define DEFAULT_PMT_PID 0x66
