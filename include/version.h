#pragma once

/*
 * The official version number of this set of software.
 *
 */

#undef DEBUG
#include "printing.h"

#define TSTOOLS_VERSION 2.0
#define STRINGIZE1(x) #x
#define STRINGIZE(x) STRINGIZE1(x)
#define TSTOOLS_VERSION_STRING STRINGIZE(TSTOOLS_VERSION)

const char software_version[] = TSTOOLS_VERSION_STRING;

// The following is intended to be output as part of the main help text for
// each program. ``program_name`` is thus the name of the program.
#define REPORT_VERSION(program_name)                                                              \
    fprint_msg("  TS tools version %s, %s built %s %s\n", software_version, (program_name),       \
        __DATE__, __TIME__)
