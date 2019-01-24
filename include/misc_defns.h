#pragma once

/*
 * Miscellaneous useful definitions
 *
 */

#include <cmath>
#include <string>

#include "tswrite_defns.h"
#include "video_defns.h"

// Some (internal) functions find it convenient to have a union of the
// possible output streams. Rather than duplicate the definition of these,
// we put them here...
union _writer {
    FILE* es_output; // output to an ES file
    TS_writer_p ts_output; // output via a TS writer
};
typedef union _writer WRITER;

// A simple macro to return a bit from a bitfield, for use in printf()
#define ON(byt, msk) ((byt & msk) ? 1 : 0)
