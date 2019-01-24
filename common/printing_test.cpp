/*
 * Test the print redirection facilities
 *
 */

#include <cstdio>

#include "printing.h"
#include "version.h"

// Some example redirection routines

// static void print_message_to_stdout(const char* message) { (void)printf("<<<MSG>>> %s",
// message); } static void print_message_to_stderr(const char* message) { (void)printf("<<<ERR>>>
// %s", message); } static void fprint_message_to_stdout(const char* format, va_list arg_ptr)
//{
//    printf("<<<MSG>>> ");
//    (void)vfprintf(stdout, format, arg_ptr);
//}
// static void fprint_message_to_stderr(const char* format, va_list arg_ptr)
//{
//    printf("<<<ERR>>> ");
//    (void)vfprintf(stdout, format, arg_ptr);
//}

static void print_usage()
{
    printf("Usage: test_printing\n"
           "\n");
    REPORT_VERSION("test_printing");
    printf("\n"
           "  Test the print redirection facilities.\n");
}

int main(int argc, char** argv)
{
    // int err;

    if (argc > 1) {
        print_usage();
        return 1;
    }

    printf("A fairly crude set of tests, mainly to check that nothing falls over.\n");
    printf("For each set of tests, you should see 4 messages, all very similar.\n");

    printf("------------------------------------\n");
    printf("Testing the default output functions\n");
    printf("------------------------------------\n");
    print_msg("1. Printing a normal message\n");
    print_err("2. Printing an error message\n");
    fprint_msg("3. Printing a formatted '%s'\n", "message");
    fprint_err("4. Printing a formatted '%s'\n", "error");

    printf("-------------------------------------------\n");
    printf("Choosing 'traditional' output and repeating\n");
    printf("-------------------------------------------\n");
    redirect_output_stderr();
    print_msg("1. Printing a normal message\n");
    print_err("2. Printing an error message\n");
    fprint_msg("3. Printing a formatted '%s'\n", "message");
    fprint_err("4. Printing a formatted '%s'\n", "error");

    printf("---------------------------------------------\n");
    printf("Choosing 'all output to stdout' and repeating\n");
    printf("---------------------------------------------\n");
    redirect_output_stdout();
    print_msg("1. Printing a normal message\n");
    print_err("2. Printing an error message\n");
    fprint_msg("3. Printing a formatted '%s'\n", "message");
    fprint_err("4. Printing a formatted '%s'\n", "error");

    // printf("-----------------------------------------\n");
    // printf("Choosing 'custom functions' and repeating\n");
    // printf("-----------------------------------------\n");
    // err = redirect_output(print_message_to_stdout, print_message_to_stderr,
    //    fprint_message_to_stdout, fprint_message_to_stderr);
    // if (err) {
    //    printf("Oops -- that went wrong: %d\n", err);
    //    return 1;
    //}
    // print_msg("1. Printing a normal message\n");
    // print_err("2. Printing an error message\n");
    // fprint_msg("3. Printing a formatted '%s'\n", "message");
    // fprint_err("4. Printing a formatted '%s'\n", "error");

    // printf("---------------------------------------------\n");
    // printf("Trying to choose only *some* custom functions\n");
    // printf("---------------------------------------------\n");
    // err = redirect_output(
    //    print_message_to_stdout, print_message_to_stderr, nullptr, fprint_message_to_stderr);
    // if (err == 0) {
    //    printf("Oh dear, that appeared to work: %d\n", err);
    //    printf("So what happens if we try our tests again?\n");
    //    print_msg("1. Printing a normal message\n");
    //    print_err("2. Printing an error message\n");
    //    fprint_msg("3. Printing a formatted '%s'\n", "message");
    //    fprint_err("4. Printing a formatted '%s'\n", "error");
    //    return 1;
    //}
    // printf("Which failed - good\n");

    // printf("------------------------------------------------------------------\n");
    // printf("After that (expected) failure, all four messages should still work\n");
    // printf("------------------------------------------------------------------\n");
    // print_msg("1. Printing a normal message\n");
    // print_err("2. Printing an error message\n");
    // fprint_msg("3. Printing a formatted '%s'\n", "message");
    // fprint_err("4. Printing a formatted '%s'\n", "error");

    return 0;
}

// Local Variables:
// tab-width: 8
// indent-tabs-mode: nil
// c-basic-offset: 2
// End:
// vim: set tabstop=8 shiftwidth=2 expandtab:
