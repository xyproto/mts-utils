#pragma once

/*
 * Infrastructure to handle byte data as bit data, and particularly to read
 * Exp-Golomb encoded data.
 *
 * See H.264 clause 10.
 *
 */

#include "compat.h"

struct bitdata {
    byte* data; // The data we're reading from
    int data_len; // It's length
    int cur_byte; // Which byte our current bit is in
    int cur_bit; // Which bit within that byte
};
typedef struct bitdata* bitdata_p;
#define SIZEOF_BITDATA sizeof(struct bitdata)
