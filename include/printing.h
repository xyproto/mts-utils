#pragma once

/*
 * Support for printing out to stdout/stderr/elsewhere -- functions to use
 * instead of printf, etc.
 *
 */

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <string>

/*
 * Support for printing out to stdout/stderr/elsewhere -- functions to use
 * instead of printf, etc.
 *
 */

using namespace std::string_literals;

// ============================================================
// Default printing functions
// ============================================================

inline void print_message_to_stdout(const std::string message) { std::cout << message; }

inline void print_message_to_stdout(const char* message) { std::cout << message; }

inline void print_message_to_stderr(const std::string message) { std::cerr << message; }

inline void print_message_to_stderr(const char* message) { std::cerr << message; }

static void fprint_message_to_stdout(const char* fmt, va_list arg_ptr) { vprintf(fmt, arg_ptr); }

static void fprint_message_to_stderr(const char* fmt, va_list arg_ptr)
{
    vfprintf(stderr, fmt, arg_ptr);
}

static void flush_stdout(void) { std::cout << std::flush; }

// ============================================================
// Print redirection defaults to all output going to stdout
// ============================================================

struct print_fns {
    void (*print_message_fn)(const char* message);
    void (*print_error_fn)(const char* message);

    void (*fprint_message_fn)(const char* format, va_list arg_ptr);
    void (*fprint_error_fn)(const char* format, va_list arg_ptr);

    void (*flush_message_fn)(void);
};

static struct print_fns fns = { print_message_to_stdout, print_message_to_stdout,
    fprint_message_to_stdout, fprint_message_to_stdout, flush_stdout };

// ============================================================
// Functions for printing
// ============================================================

/*
 * Print the given string, as a normal message.
 */
void print_msg(const std::string message) { fns.print_message_fn(message.c_str()); }

/*
 * Print the given string, as a normal message.
 */
void print_msg(const char* message) { fns.print_message_fn(message); }

/*
 * Print the given string, as an error message.
 */
void print_err(const std::string message) { fns.print_error_fn(message.c_str()); }

/*
 * Print the given string, as an error message.
 */
void print_err(const char* message) { fns.print_error_fn(message); }

/*
 * Prints the given format text, as a normal message.
 */
void fprint_msg(const char* fmt, ...)
{
    va_list va_arg;
    va_start(va_arg, fmt);
    fns.fprint_message_fn(fmt, va_arg);
    va_end(va_arg);
}

/*
 * Prints the given format text, as a normal message.
 */
void fprint_msg(const std::string fmt, ...)
{
    va_list va_arg;
    va_start(va_arg, fmt);
    fns.fprint_message_fn(fmt.c_str(), va_arg);
    va_end(va_arg);
}

/*
 * Prints the given formatted text, as an error message.
 */
void fprint_err(const char* fmt, ...)
{
    va_list va_arg;
    va_start(va_arg, fmt);
    fns.fprint_error_fn(fmt, va_arg);
    va_end(va_arg);
}

/*
 * Prints the given formatted text, as an error message.
 */
void fprint_err(const std::string fmt, ...)
{
    va_list va_arg;
    va_start(va_arg, fmt);
    fns.fprint_error_fn(fmt.c_str(), va_arg);
    va_end(va_arg);
}

void print_msg_or_err(bool is_msg, const std::string message) {
    if (is_msg) {
        std::cout << message;
        //fns.fprint_message_fn("%s", message.c_str());
    } else {
        //fns.fprint_error_fn("%s", message.c_str());
        std::cerr << message;
    }
}

void print_msg_or_err(bool is_msg, const char* message) {
    if (is_msg) {
        std::cout << message;
        //fns.fprint_message_fn("%s", message);
    } else {
        //fns.fprint_error_fn("%s", message);
        std::cerr << message;
    }
}

/*
 * Prints the given formatted text, as a normal or error message.
 * If `is_msg`, then as a normal message, else as an error
 */
void fprint_msg_or_err(bool is_msg, const char* fmt, ...)
{
    va_list va_arg;
    va_start(va_arg, fmt);
    if (is_msg) {
        fns.fprint_message_fn(fmt, va_arg);
    } else {
        fns.fprint_error_fn(fmt, va_arg);
    }
    va_end(va_arg);
}

/*
 * Prints the given formatted text, as a normal or error message.
 * If `is_msg`, then as a normal message, else as an error
 */
void fprint_msg_or_err(bool is_msg, const std::string fmt, ...)
{
    va_list va_arg;
    va_start(va_arg, fmt);
    if (is_msg) {
        fns.fprint_message_fn(fmt.c_str(), va_arg);
    } else {
        fns.fprint_error_fn(fmt.c_str(), va_arg);
    }
    va_end(va_arg);
}

/*
 * Prints the given string, as a normal message.
 */
void flush_msg(void) { fns.flush_message_fn(); }

// ============================================================
// Choosing what the printing functions do
// ============================================================

/*
 * Calling this causes errors to go to stderr, and all other output
 * to go to stdout. This is the "traditional" mechanism used by
 * Unices.
 */
void redirect_output_stderr(void)
{
    fns.print_message_fn = &print_message_to_stdout;
    fns.print_error_fn = &print_message_to_stderr;
    fns.fprint_message_fn = &fprint_message_to_stdout;
    fns.fprint_error_fn = &fprint_message_to_stderr;
    fns.flush_message_fn = &flush_stdout;
}

/*
 * Calling this causes all output to go to stdout. This is simpler,
 * and is likely to be more use to most users.
 *
 * This is the default state.
 */
void redirect_output_stdout(void)
{
    fns.print_message_fn = &print_message_to_stdout;
    fns.print_error_fn = &print_message_to_stdout;
    fns.fprint_message_fn = &fprint_message_to_stdout;
    fns.fprint_error_fn = &fprint_message_to_stdout;
    fns.flush_message_fn = &flush_stdout;
}

void test_C_printing(void)
{
    std::cout << "C Message" << std::endl;
    std::cerr << "C Error" << std::endl;
    fprint_msg("C Message %s\n", "Fred");
    fprint_err("C Error %s\n", "Fred");
}
