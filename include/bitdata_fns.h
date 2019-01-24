#pragma once

/*
 * Infrastructure to handle byte data as bit data, and particularly to read
 * Exp-Golomb encoded data.
 *
 * See H.264 clause 10.
 *
 */

#include "bitdata_defns.h"

/*
 * Build a new bitdata datastructure.
 *
 * - `data` is the byte array we're extracting bits from.
 * - `data_len` is its length (in bytes).
 *
 * Returns 0 if it succeeds, 1 if some error occurs.
 */
int build_bitdata(bitdata_p* bitdata, byte data[], size_t data_len);

/*
 * Tidy up and free a bitdata datastructure after we've finished with it.
 *
 * Clears the bitdata datastructure, frees it, and sets `bitdata` to nullptr.
 */
void free_bitdata(bitdata_p* bitdata);

/*
 * Return the next bit from the data.
 *
 * Returns 0 if it reads the bit correctly, 1 if there are no more
 * bits to be read.
 */
int read_bit(bitdata_p bitdata, byte* bit);

/*
 * Reads `count` bits from the data.
 *
 * Note it is asserted that `count` must be in the range 0..32.
 *
 * Returns 0 if all went well, 1 if there were not enough bits in the data.
 */
int read_bits(bitdata_p bitdata, int count, uint32_t* bits);

/*
 * Reads `count` bits from the data, into a byte.
 *
 * Note it is asserted that `count` must be in the range 0..8.
 *
 * Returns 0 if all went well, 1 if there were not enough bits in the data.
 */
int read_bits_into_byte(bitdata_p bitdata, int count, byte* bits);
/*
 * Read zero bits, counting them. Stop at the first non-zero bit.
 *
 * Returns the number of zero bits. Note that the non-zero bit is not
 * "unread" in any way, so reading another bit will retrieve the first bit
 * thereafter.
 */
int count_zero_bits(bitdata_p bitdata);

/*
 * Read and decode an Exp-Golomb code.
 *
 * Reference H.264 10.1 for an explanation.
 *
 * Returns 0 if all went well, 1 if there were not enough bits in the data.
 */
int read_exp_golomb(bitdata_p bitdata, uint32_t* result);

/*
 * Read and decode a signed Exp-Golomb code.
 *
 * Reference H.264 10.1 sqq for an explanation.
 *
 * Returns 0 if all went well, 1 if there were not enough bits in the data.
 */
int read_signed_exp_golomb(bitdata_p bitdata, int32_t* result);
