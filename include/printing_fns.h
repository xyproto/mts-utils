#pragma once

/*
 * Support for printing out to stdout/stderr/elsewhere -- functions to use
 * instead of printf, etc.
 *
 */

#include <cstdarg>
#include <cstdio>
#include <string>

using namespace std::string_literals;

// ============================================================
// Functions for printing
// ============================================================
/*
 * Prints the given string, as a normal message.
 */
void print_msg(const std::string text);
/*
 * Prints the given string, as an error message.
 */
void print_err(const std::string text);
/*
 * Prints the given format text, as a normal message.
 */
void fprint_msg(const char* format, ...);
/*
 * Prints the given formatted text, as an error message.
 */
void fprint_err(const char* format, ...);
/*
 * Prints the given formatted text, as a normal or error message.
 * If `is_msg`, then as a normal message, else as an error
 */
void fprint_msg_or_err(bool is_msg, const char* format, ...);
/*
 * Flush the message output
 */
void flush_msg(void);

// ============================================================
// Choosing what the printing functions do
// ============================================================
/*
 * Calling this causes errors to go to stderr, and all other output
 * to go to stdout. This is the "traditional" mechanism used by
 * Unices.
 */
void redirect_output_stderr(void);
/*
 * Calling this causes all output to go to stdout. This is simpler,
 * and is likely to be more use to most users.
 *
 * This is the default state.
 */
void redirect_output_stdout(void);
/*
 * This allows the user to specify a set of functions to use for
 * formatted printing and non-formatted printing of errors and
 * other messages.
 *
 * It is up to the caller to ensure that all of the functions
 * make sense. All four functions must be specified.
 *
 * * `new_print_message_fn` takes a string and prints it out to the "normal"
 *    output stream.
 * * `new_print_error_fn` takes a string and prints it out to the error output
 *    stream.
 * * `new_fprint_message_fn` takes a printf-style format string and the
 *    appropriate arguments, and writes the result out to the "normal" output.
 * * `new_fprint_error_fn` takes a printf-style format string and the
 *    appropriate arguments, and writes the result out to the "error" output.
 * * `new_flush_msg_fn` flushes the "normal" message output.
 *
 * Returns 0 if all goes well, 1 if something goes wrong.
 */
int redirect_output(void (*new_print_message_fn)(const char* message),
    void (*new_print_error_fn)(const char* message),
    void (*new_fprint_message_fn)(const char* format, va_list arg_ptr),
    void (*new_fprint_error_fn)(const char* format, va_list arg_ptr),
    void (*new_flush_msg_fn)(void));

// Just for the moment
void test_C_printing(void);
