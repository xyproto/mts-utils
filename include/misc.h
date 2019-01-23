#pragma once

/*
 * Miscellaneous useful functions.
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

#include <cstdio>
#include <cstdlib>

// For the command line utilities
#include <cerrno>
#include <climits>
#include <cstring>
#include <fcntl.h> // O_... flags

//#include <cmath>

#include "version.h"

// For the socket handling
#include <arpa/inet.h> // inet_aton
#include <netdb.h>
#include <netinet/in.h> // sockaddr_in
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // open, close

#include "compat.h"
#include "es_fns.h"
#include "misc_fns.h"
#include "pes_fns.h"
#include "printing_fns.h"

#define DEBUG_SEEK 1

// ============================================================
// CRC calculation
// ============================================================

static uint32_t crc_table[256];

/*
 * Populate the (internal) CRC table. May safely be called more than once.
 */
static void make_crc_table(void)
{
    int i, j;
    int already_done = 0;
    uint32_t crc;

    if (already_done)
        return;
    else
        already_done = 1;

    for (i = 0; i < 256; i++) {
        crc = i << 24;
        for (j = 0; j < 8; j++) {
            if (crc & 0x80000000L)
                crc = (crc << 1) ^ CRC32_POLY;
            else
                crc = (crc << 1);
        }
        crc_table[i] = crc;
    }
}

/*
 * Compute CRC32 over a block of data, by table method.
 *
 * Returns a working value, suitable for re-input for further blocks
 *
 * Notes: Input value should be 0xffffffff for the first block,
 *        else return value from previous call (not sure if that
 *        needs complementing before being passed back in).
 */
uint32_t crc32_block(uint32_t crc, byte* pData, int blk_len)
{
    static int table_made = FALSE;
    int i, j;

    if (!table_made)
        make_crc_table();

    for (j = 0; j < blk_len; j++) {
        i = ((crc >> 24) ^ *pData++) & 0xff;
        crc = (crc << 8) ^ crc_table[i];
    }
    return crc;
}

/*
 * Print out (the first `max`) bytes of a byte array.
 *
 * - if `is_msg` then print as a message, otherwise as an error
 * - `name` is identifying text to start the report with.
 * - `data` is the byte data to print. This may be nullptr.
 * - `length` is its length
 * - `max` is the maximum number of bytes to print
 *
 * Prints out::
 *
 *    <name> (<length>): b1 b2 b3 b4 ...
 *
 * where no more than `max` bytes are to be printed (and "..." is printed
 * if not all bytes were shown).
 */
void print_data(int is_msg, const char* name, const byte data[], int length, int max)
{
    int ii;

    if (length == 0) {
        fprint_msg_or_err(is_msg, "%s (0 bytes)\n", name);
        return;
    }

#define MAX_LINE_LENGTH 80

    fprint_msg_or_err(is_msg, "%s (%d byte%s):", name, length, (length == 1 ? "" : "s"));
    if (data == nullptr)
        fprint_msg_or_err(is_msg, " <null>"); // Shouldn't happen, but let's be careful.
    else {
        for (ii = 0; ii < (length < max ? length : max); ii++)
            fprint_msg_or_err(is_msg, " %02x", data[ii]);
        if (max < length)
            fprint_msg_or_err(is_msg, "...");
    }
    fprint_msg_or_err(is_msg, "\n");
}

/*
 * Print out (the last `max`) bytes of a byte array.
 *
 * - `name` is identifying text to start the report with.
 * - `data` is the byte data to print. This may be nullptr.
 * - `length` is its length
 * - `max` is the maximum number of bytes to print
 *
 * Prints out::
 *
 *    <name> (<length>): ... b1 b2 b3 b4
 *
 * where no more than `max` bytes are to be printed (and "..." is printed
 * if not all bytes were shown).
 */
void print_end_of_data(char* name, byte data[], int length, int max)
{
    int ii;
    if (length == 0) {
        fprint_msg("%s (0 bytes)\n", name);
        return;
    }

    fprint_msg("%s (%d byte%s):", name, length, (length == 1 ? "" : "s"));
    if (data == nullptr)
        print_msg(" <null>"); // Shouldn't happen, but let's be careful.
    else {
        if (max < length)
            print_msg(" ...");
        for (ii = (length < max ? 0 : length - max); ii < length; ii++)
            fprint_msg(" %02x", data[ii]);
    }
    print_msg("\n");
}

/*
 * Print out the bottom N bits from a byte
 */
void print_bits(int num_bits, byte value)
{
    int ii;
    byte masks[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
    for (ii = 8 - num_bits; ii < 8; ii++) {
        fprint_msg("%d", ((value & masks[ii]) >> (8 - ii - 1)));
    }
}

/*
 * Calculate log2 of `x` - for some reason this is missing from <math.h>
 */
double log2(double x)
{
    if (x == 2.0)
        return 1.0;
    else
        return log10(x) / log10(2);
}

// ============================================================
// Simple file I/O utilities
// ============================================================
/*
 * Read a given number of bytes from a file.
 *
 * This is a jacket for `read`, allowing for the future possibility of
 * buffered input, and simplifying error handling.
 *
 * - `input` is the file descriptor for the file
 * - `num_bytes` is how many bytes to read
 * - `data` is the buffer to read the bytes into
 *
 * Returns 0 if all goes well, EOF if end of file was read, or 1 if some
 * other error occurred (in which case it will already have output a message
 * on stderr about the problem).
 */
int read_bytes(int input, int num_bytes, byte* data)
{
    ssize_t total = 0;
    ssize_t length;

    // Make some allowance for short reads - for instance, if we're reading
    // from a pipe and going just a bit faster than the sender
    while (total < num_bytes) {
        length = read(input, &(data[total]), num_bytes - total);
        if (length == 0)
            return EOF;
        else if (length == -1) {
            fprint_err("### Error reading %d bytes: %s\n", num_bytes, strerror(errno));
            return 1;
        }
        total += length;
    }
    return 0;
}

/*
 * Utility function to seek within a file
 *
 * - `filedes` is the file to seek within
 * - `posn` is the position to which to seek
 *
 * This is a jacket for::
 *
 *    new_posn = lseek(filedes,posn,SEEK_SET);
 *
 * Returns 0 if all went well, 1 if the seek failed (either because
 * it returned -1, or because the position reached was not the position
 * requested). If an error occurs, then an explanatory message will
 * already have been written to stderr.
 */
int seek_file(int filedes, offset_t posn)
{
    offset_t newposn = lseek(filedes, posn, SEEK_SET);
    if (newposn == -1) {
        fprint_err("### Error moving (seeking) to position " OFFSET_T_FORMAT " in file: %s\n",
            posn, strerror(errno));
        return 1;
    } else if (newposn != posn) {
        fprint_err("### Error moving (seeking) to position " OFFSET_T_FORMAT
                   " in file: actually moved to " OFFSET_T_FORMAT "\n",
            posn, newposn);
        return 1;
    }
    return 0;
}

/*
 * Utility function to report the current location within a file
 *
 * - `filedes` is the file to seek within
 *
 * This is a jacket for::
 *
 *    posn = lseek(filedes,0,SEEK_CUR);
 *
 * Returns the current position in the file if all went well, otherwise
 * -1 (in which case an error message will already have been written
 * on stderr)
 */
offset_t tell_file(int filedes)
{
    offset_t newposn = lseek(filedes, 0, SEEK_CUR);
    if (newposn == -1)
        fprint_err("### Error determining current position in file: %s\n", strerror(errno));
    return newposn;
}

/*
 * Utility function to open a file (descriptor), and report any errors
 *
 * This is intended only for very simple usage, and is not mean to be
 * a general purpose "open" replacement.
 *
 * - `filename` is the name of the file to open
 * - `for_write` should be TRUE if the file is to be written to,
 *   in which case it will be opened with flags O_WRONLY|O_CREAT|O_TRUNC,
 *   or FALSE if the file is to be read, in which case it will be
 *   opened with flag O_RDONLY. In both cases, on Windows the flag
 *   O_BINARY will also be set.
 *
 * Returns the file descriptor for the file, or -1 if it failed to open
 * the file.
 */
int open_binary_file(char* filename, int for_write)
{
    int flags = 0;
    int filedes;
    if (for_write) {
        flags = flags | O_WRONLY | O_CREAT | O_TRUNC;
        filedes = open(filename, flags, 00777);
    } else {
        flags = flags | O_RDONLY;
        filedes = open(filename, flags);
    }
    if (filedes == -1)
        fprint_err("### Error opening file %s for %s: %s\n", filename,
            (for_write ? "write" : "read"), strerror(errno));
    return filedes;
}

/*
 * Utility function to close a file (descriptor), and report any errors
 *
 * Does nothing if filedes is -1 or STDIN_FILENO
 *
 * Returns 0 if all went well, 1 if an error occurred.
 */
int close_file(int filedes)
{
    int err;

    if (filedes == -1 || filedes == STDIN_FILENO)
        return 0;

    err = close(filedes);
    if (err) {
        fprint_err("### Error closing file: %s\n", strerror(errno));
        return 1;
    } else
        return 0;
}

// ============================================================
// More complex file I/O utilities
// ============================================================
static int open_input_as_ES_using_PES(
    char* name, int quiet, int force_stream_type, int want_data, int* is_data, ES_p* es)
{
    int err;
    PES_reader_p reader = nullptr;

    if (name == nullptr) {
        print_err("### Cannot use standard input to read PES\n");
        return 1;
    }

    err = open_PES_reader(name, !quiet, !quiet, &reader);
    if (err) {
        fprint_err("### Error trying to build PES reader for input file %s\n", name);
        return 1;
    }
    err = build_elementary_stream_PES(reader, es);
    if (err) {
        fprint_err("### Error trying to build ES reader from PES reader\n"
                   "    for input file %s\n",
            name);
        (void)close_PES_reader(&reader);
        return 1;
    }

    if (!quiet)
        fprint_msg("Reading from %s\n", name);

    if (force_stream_type) {
        if (force_stream_type)
            *is_data = want_data;
        else
            *is_data = VIDEO_H262;
        if (!quiet)
            fprint_msg("Reading input as %s\n",
                (*is_data == VIDEO_H262
                        ? "MPEG-2 (H.262)"
                        : *is_data == VIDEO_H264 ? "MPEG-4/AVC (H.264)"
                                                 : *is_data == VIDEO_AVS ? "AVS" : "???"));
    } else {
        *is_data = reader->video_type;
    }
    return 0;
}

static int open_input_as_ES_direct(
    char* name, int quiet, int force_stream_type, int want_data, int* is_data, ES_p* es)
{
    int err;
    int use_stdin = (name == nullptr);
    int input = -1;

    if (use_stdin) {
        input = STDIN_FILENO;
    } else {
        input = open_binary_file(name, FALSE);
        if (input == -1)
            return 1;
    }

    err = build_elementary_stream_file(input, es);
    if (err) {
        fprint_err("### Error building elementary stream for %s\n", use_stdin ? "<stdin>" : name);
        if (!use_stdin)
            (void)close_file(input);
        return 1;
    }

    if (!quiet)
        fprint_msg("Reading from %s\n", (use_stdin ? "<stdin>" : name));

    if (force_stream_type || use_stdin) {
        if (force_stream_type)
            *is_data = want_data;
        else
            *is_data = VIDEO_H262;
        if (!quiet)
            fprint_msg("Reading input as %s\n",
                (*is_data == VIDEO_H262
                        ? "MPEG-2 (H.262)"
                        : *is_data == VIDEO_H264 ? "MPEG-4/AVC (H.264)"
                                                 : *is_data == VIDEO_AVS ? "AVS" : "???"));
    } else {
        int video_type;
        err = decide_ES_video_type(*es, FALSE, FALSE, &video_type);
        if (err) {
            fprint_err("### Error deciding on stream type for file %s\n", name);
            close_elementary_stream(es);
            return 1;
        }

        // We want to rewind, to "unread" the bytes we read to decide our filetype.
        // The easiest way to do that and return to our initial conditions is to
        // recreate our ES context
        free_elementary_stream(es);

        err = seek_file(input, 0);
        if (err) {
            print_err("### Error returning to start position in file after"
                      " working out video type\n");
            (void)close_file(input);
            return 1;
        }

        err = build_elementary_stream_file(input, es);
        if (err) {
            fprint_err("### Error (re)building elementary stream for %s\n", name);
            return 1;
        }

        *is_data = video_type;
        if (!quiet)
            fprint_msg("Input appears to be %s\n",
                (*is_data == VIDEO_H262 ? "MPEG-2 (H.262)"
                                        : *is_data == VIDEO_H264 ? "MPEG-4/AVC (H.264)"
                                                                 : *is_data == VIDEO_AVS
                                ? "AVS"
                                : *is_data == VIDEO_UNKNOWN ? "Unknown" : "???"));
    }
    return 0;
}

/*
 * Open an input file appropriately for reading as ES.
 *
 * - `name` is the name of the file, or nullptr if standard input
 *   is to be read from (which is not allowed if `use_pes` is
 *   TRUE).
 *
 * - If `use_pes` is true then the input file is PS or TS and should
 *   be read via a PES reader.
 *
 * - If `quiet` is true then information about the file being read will
 *   not be written out. Otherwise, its name and what is decided about
 *   its content will be printed.
 *
 * - If `force_stream_type` is true, then the caller asserts that
 *   the input shall be read according to `want_data`, and not whatever
 *   might be deduced from looking at the file itself.
 *
 * - If `force_stream_type` is true, then `want_data` should be one of
 *   VIDEO_H262, VIDEO_H264 or VIDEO_AVS. `is_data` will then be
 *   returned with the same value.
 *
 * - If `force_stream_type` is false, then the function will attempt
 *   to determine what type of data it has, and `is_data` will be set
 *   to whatever is determined (presumably one of VIDEO_H262, VIDEO_H264
 *   or VIDEO_AVS). It if cannot decide, then it will set it to VIDEO_UNKNOWN.
 *
 * - If input is from standard input, and `force_stream_type` is FALSE,
 *   `is_data` will always be set to VIDEO_H262, which may be incorrect.
 *
 * - `es` is the new ES reader context.
 *
 * Returns 0 if all goes well, 1 if something goes wrong. In the latter case,
 * suitable messages will have been written out to standard error.
 */
int open_input_as_ES(char* name, int use_pes, int quiet, int force_stream_type, int want_data,
    int* is_data, ES_p* es)
{
    if (use_pes)
        return open_input_as_ES_using_PES(name, quiet, force_stream_type, want_data, is_data, es);
    else
        return open_input_as_ES_direct(name, quiet, force_stream_type, want_data, is_data, es);
}

/*
 * Close an input ES stream opened with `open_input_as_ES`.
 *
 * Specifically, this will close the ES stream and also any underlying PES
 * reader and file (unless the input was standard input).
 *
 * - `name` is the name of the file, used for error reporting.
 * - `es` is the ES stream to close. This will be set to nullptr.
 *
 * Returns 0 if all goes well, 1 if something goes wrong. In the latter case,
 * suitable messages will have been written out to standard error.
 */
int close_input_as_ES(char* name, ES_p* es)
{
    if (!(*es)->reading_ES) {
        int err = close_PES_reader(&(*es)->reader);
        if (err) {
            fprint_err("### Error closing PES reader for file %s\n", name);
            close_elementary_stream(es);
            return 1;
        }
    }
    close_elementary_stream(es);
    return 0;
}

// ============================================================
// Command line "helpers"
// ============================================================
/*
 * Read in an unsigned integer value, checking for extraneous characters.
 *
 * - `prefix` is an optional prefix for error messages, typically the
 *   name of the program. It may be nullptr.
 * - `cmd` is the command switch we're reading for (typically ``argv[ii]``),
 *   which is used in error messages.
 * - `str` is the string to read (typically ``argv[ii+1]``).
 * - `base` is the base to read to. If it is 0, then the user can use
 *   C-style expressions like "0x68" to specify the base on the command line.
 * - `value` is the value read.
 *
 * Returns 0 if all went well, 1 otherwise (in which case a message
 * explaining will have been written to stderr).
 */
int unsigned_value(char* prefix, char* cmd, char* arg, int base, uint32_t* value)
{
    char* ptr;
    unsigned long val;
    errno = 0;
    val = strtoul(arg, &ptr, base);
    if (errno) {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        if (errno == ERANGE && val == 0)
            fprint_err(
                "String cannot be converted to (long) unsigned integer in %s %s\n", cmd, arg);
        else if (errno == ERANGE && (val == LONG_MAX || val == (long unsigned int)LONG_MIN))
            fprint_err("Number is too big (overflows) in %s %s\n", cmd, arg);
        else
            fprint_err("Cannot read number in %s %s (%s)\n", cmd, arg, strerror(errno));
        return 1;
    }
    if (ptr[0] != '\0') {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        if (ptr - arg == 0)
            fprint_err("Argument to %s should be a number, in %s %s\n", cmd, cmd, arg);
        else
            fprint_err("Unexpected characters ('%s') after the %.*s in %s %s\n", ptr,
                (int)(ptr - arg), arg, cmd, arg);
        return 1;
    }

    *value = val;
    return 0;
}

/*
 * Read in an integer value, checking for extraneous characters.
 *
 * - `prefix` is an optional prefix for error messages, typically the
 *   name of the program. It may be nullptr.
 * - `cmd` is the command switch we're reading for (typically ``argv[ii]``),
 *   which is used in error messages.
 * - `str` is the string to read (typically ``argv[ii+1]``).
 * - if `positive` is true, then the number read must be positive (0 or more).
 * - `base` is the base to read to. If it is 0, then the user can use
 *   C-style expressions like "0x68" to specify the base on the command line.
 * - `value` is the value read.
 *
 * Returns 0 if all went well, 1 otherwise (in which case a message
 * explaining will have been written to stderr).
 */
int int_value(
    const char* prefix, const char* cmd, const char* arg, int positive, int base, int* value)
{
    char* ptr;
    long val;
    errno = 0;
    val = strtol(arg, &ptr, base);
    if (errno) {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        if (errno == ERANGE && val == 0)
            fprint_err("String cannot be converted to (long) integer in %s %s\n", cmd, arg);
        else if (errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
            fprint_err("Number is too big (overflows) in %s %s\n", cmd, arg);
        else
            fprint_err("Cannot read number in %s %s (%s)\n", cmd, arg, strerror(errno));
        return 1;
    }
    if (ptr[0] != '\0') {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        if (ptr - arg == 0)
            fprint_err("Argument to %s should be a number, in %s %s\n", cmd, cmd, arg);
        else
            fprint_err("Unexpected characters ('%s') after the %.*s in %s %s\n", ptr,
                (int)(ptr - arg), arg, cmd, arg);
        return 1;
    }

    if (val > INT_MAX || val < INT_MIN) {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        fprint_err("Value %ld (in %s %s) is too large (to fit into 'int')\n", val, cmd, arg);
        return 1;
    }

    if (positive && val < 0) {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        fprint_err("Value %ld (in %s %s) is less than zero\n", val, cmd, arg);
        return 1;
    }

    *value = val;
    return 0;
}

/*
 * Read in an integer value, checking for extraneous characters and a range.
 *
 * - `prefix` is an optional prefix for error messages, typically the
 *   name of the program. It may be nullptr.
 * - `cmd` is the command switch we're reading for (typically ``argv[ii]``),
 *   which is used in error messages.
 * - `str` is the string to read (typically ``argv[ii+1]``).
 * - `minimum` is the minimum value allowed.
 * - `maximum` is the maximum value allowed.
 * - `base` is the base to read to. If it is 0, then the user can use
 *   C-style expressions like "0x68" to specify the base on the command line.
 * - `value` is the value read.
 *
 * Returns 0 if all went well, 1 otherwise (in which case a message
 * explaining will have been written to stderr).
 */
int int_value_in_range(
    char* prefix, char* cmd, char* arg, int minimum, int maximum, int base, int* value)
{
    int err, temp;
    err = int_value(prefix, cmd, arg, (minimum >= 0), base, &temp);
    if (err)
        return err;

    if (temp > maximum || temp < minimum) {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        fprint_err("Value %d (in %s %s) is not in range %d..%d (0x%x..0x%x)\n", temp, cmd, arg,
            minimum, maximum, minimum, maximum);
        return 1;
    }
    *value = temp;
    return 0;
}

/*
 * Read in a double value, checking for extraneous characters.
 *
 * - `prefix` is an optional prefix for error messages, typically the
 *   name of the program. It may be nullptr.
 * - `cmd` is the command switch we're reading for (typically ``argv[ii]``),
 *   which is used in error messages.
 * - `str` is the string to read (typically ``argv[ii+1]``).
 * - if `positive` is true, then the number read must be positive (0 or more).
 * - `value` is the value read.
 *
 * Returns 0 if all went well, 1 otherwise (in which case a message
 * explaining will have been written to stderr).
 */
int double_value(char* prefix, char* cmd, char* arg, int positive, double* value)
{
    char* ptr;
    double val;
    errno = 0;
    val = strtod(arg, &ptr);
    if (errno) {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        if (errno == ERANGE && val == 0)
            fprint_err("String cannot be converted to (double) float in %s %s\n", cmd, arg);
        else if (errno == ERANGE && (val == HUGE_VAL || val == -HUGE_VAL))
            fprint_err("Number is too big (overflows) in %s %s\n", cmd, arg);
        else
            fprint_err("Cannot read number in %s %s (%s)\n", cmd, arg, strerror(errno));
        return 1;
    }
    if (ptr[0] != '\0') {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        fprint_err("Unexpected characters ('%s') after the %.*s in %s %s\n", ptr, (int)(ptr - arg),
            arg, cmd, arg);
        return 1;
    }

    if (positive && val < 0) {
        print_err("### ");
        if (prefix != nullptr)
            fprint_err("%s: ", prefix);
        fprint_err("Value %f (in %s %s) is less than zero\n", val, cmd, arg);
        return 1;
    }

    *value = val;
    return 0;
}

/*
 * Read in a hostname and (optional) port
 *
 * - `prefix` is an optional prefix for error messages, typically the
 *   name of the program. It may be nullptr.
 * - `cmd` is the command switch we're reading for (typically ``argv[ii]``),
 *   which is used in error messages. It may be nullptr if we are reading a
 *   "plain" host name, with no command switch in front of it.
 * - `arg` is the string to read (typically ``argv[ii+1]``).
 * - `hostname` is the host name read
 * - `port` is the port read (note that this is not touched if there is
 *   no port number, so it may be set to a default before calling this
 *   function)
 *
 * Note that this works by pointing `hostname` to the start of the `arg`
 * string, and then if there is a ':' in `arg`, changing that colon to
 * a '\0' delimiter, and interpreting the string thereafter as the port
 * number. If *that* fails, it resets the '\0' as a ':'.
 *
 * Returns 0 if all went well, 1 otherwise (in which case a message
 * explaining will have been written to stderr).
 */
int host_value(char* prefix, char* cmd, char* arg, char** hostname, int* port)
{
    char* p = strchr(arg, ':');

    *hostname = arg;

    if (p != nullptr) {
        char* ptr;
        p[0] = '\0'; // yep, modifying argv[ii+1]
        errno = 0;
        *port = strtol(p + 1, &ptr, 10);
        if (errno) {
            p[0] = ':';
            print_err("### ");
            if (prefix != nullptr)
                fprint_err("%s: ", prefix);
            if (cmd)
                fprint_err("Cannot read port number in %s %s (%s)\n", cmd, arg, strerror(errno));
            else
                fprint_err("Cannot read port number in %s (%s)\n", arg, strerror(errno));
            return 1;
        }
        if (ptr[0] != '\0') {
            p[0] = ':';
            print_err("### ");
            if (prefix != nullptr)
                fprint_err("%s: ", prefix);
            if (cmd)
                fprint_err("Unexpected characters in port number in %s %s\n", cmd, arg);
            else
                fprint_err("Unexpected characters in port number in %s\n", arg);
            return 1;
        }
        if (*port < 0) {
            p[0] = ':';
            print_err("### ");
            if (prefix != nullptr)
                fprint_err("%s: ", prefix);
            if (cmd)
                fprint_err("Negative port number in %s %s\n", cmd, arg);
            else
                fprint_err("Negative port number in %s\n", arg);
            return 1;
        }
    }
    return 0;
}

// ============================================================
// Socket support
// ============================================================
/*
 * Connect to a socket, to allow us to write to it, using TCP/IP.
 *
 * - `hostname` is the name of the host to connect to
 * - `port` is the port to use
 * - if `use_tcpip`, then a TCP/IP connection will be made, otherwise UDP.
 *   For UDP, multicast TTL will be enabled.
 * - If the destination address (`hostname`) is multicast and `multicast_ifaddr`
 *   is supplied, it is used to select (by IP address) the network interface
 *   on which to send the multicasts.  It may be nullptr to use the default,
 *   or for non-multicast cases.
 *
 * A socket connected to via this function must be disconnected from with
 * disconnect_socket().
 *
 *   (This is actually only crucial on Windows, where WinSock must be
 *   neatly shut down, but should also be done on Unix in case future
 *   termination code is added.)
 *
 * Returns a positive integer (the file descriptor for the socket) if it
 * succeeds, or -1 if it fails, in which case it will have complained on
 * stderr.
 */
int connect_socket(char* hostname, int port, int use_tcpip, char* multicast_ifaddr)
{
    int output;
    int result;
    struct hostent* hp;
    struct sockaddr_in ipaddr;

    output = socket(AF_INET, (use_tcpip ? SOCK_STREAM : SOCK_DGRAM), 0);
    if (output == -1) {
        fprint_err("### Unable to create socket: %s\n", strerror(errno));
        return -1;
    }

    hp = gethostbyname(hostname);
    if (hp == nullptr) {
        fprint_err("### Unable to resolve host %s: %s\n", hostname, hstrerror(h_errno));
        return -1;
    }
    memcpy(&ipaddr.sin_addr.s_addr, hp->h_addr, hp->h_length);
    ipaddr.sin_family = hp->h_addrtype;
    ipaddr.sin_port = htons(port);

    if (IN_CLASSD(ntohl(ipaddr.sin_addr.s_addr))) {
        // Needed if we're doing multicast
        byte ttl = 16;
        result = setsockopt(output, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl));
        if (result < 0) {
            fprint_err("### Error setting socket for IP_MULTICAST_TTL: %s\n", strerror(errno));
            return -1;
        }

        if (multicast_ifaddr) {
            struct in_addr addr;
            inet_aton(multicast_ifaddr, &addr);
            result = setsockopt(output, IPPROTO_IP, IP_MULTICAST_IF, (char*)&addr, sizeof(addr));
            if (result < 0) {
                fprint_err("### Unable to set multicast interface %s: %s\n", multicast_ifaddr,
                    strerror(errno));
                return -1;
            }
        }
    }

    result = connect(output, (struct sockaddr*)&ipaddr, sizeof(ipaddr));
    if (result < 0) {
        fprint_err("### Unable to connect to host %s: %s\n", hostname, strerror(errno));
        return -1;
    }
    return output;
}

/*
 * Disconnect from a socket (close it).
 *
 * Returns 0 if all goes well, 1 otherwise.
 */
int disconnect_socket(int socket)
{
    int err = close(socket);
    if (err == EOF) {
        fprint_err("### Error closing output: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

const char* ipv4_addr_to_string(const uint32_t addr)
{
    static char buf[64];

    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (addr >> 24) & 0xff, (addr >> 16) & 0xff,
        (addr >> 8) & 0xff, (addr & 0xff));
    return buf;
}

int ipv4_string_to_addr(uint32_t* dest, const char* string)
{
    char* str_cpy = strdup(string);
    int rv = 0;
    char *p, *p2;
    int val;
    int nr;
    uint32_t out = 0;

    for (nr = 0, p = str_cpy; nr < 4 && *p; p = p2 + 1, ++nr) {
        char* px = nullptr;
        p2 = strchr(p, '.');
        if (p2) {
            *p2 = '\0';
        }
        val = strtoul(p, &px, 0);
        if (px && *px) {
            return -1;
        }
        out |= (val << ((3 - nr) << 3));
    }

    (*dest) = out;
    free(str_cpy);
    return rv;
}
