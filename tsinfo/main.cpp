/*
 * Locate the PAT and PMT packets in an H.222 transport stream (TS),
 * and report on their contents (i.e., the program and stream info).
 *
 */

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#include "accessunit.h"
#include "bitdata.h"
#include "compat.h"
#include "es.h"
#include "h222.h"
#include "h262.h"
#include "misc.h"
#include "nalunit.h"
#include "pes.h"
#include "pidint.h"
#include "printing.h"
#include "ps.h"
#include "reverse.h"
#include "ts.h"
#include "tswrite.h"
#include "version.h"

/*
 * Report on the program streams, by looking at the PAT and PMT packets
 * in the first `max` TS packets of the given input stream
 *
 * Returns 0 if all went well, 1 if something went wrong.
 */
int report_streams(TS_reader_p tsreader, int max, bool verbose)
{
    int err;
    int ii;

    // TODO: Should really support multiple programs
    //       (some use of pidint_list to support program number -> PMT?)

    pidint_list_p this_prog_list = nullptr;
    pidint_list_p last_prog_list = nullptr;
    pmt_p this_pmt = nullptr;
    pmt_p last_pmt = nullptr;

    uint32_t pmt_pid = 0; // which will get "masked" by the PAT pid

    byte* pat_data = nullptr;
    int pat_data_len = 0;
    int pat_data_used = 0;

    byte* pmt_data = nullptr;
    int pmt_data_len = 0;
    int pmt_data_used = 0;

    int num_pats = 0;
    int num_pmts = 0;

    fprint_msg("Scanning %d TS packets\n", max);

    for (ii = 0; ii < max; ii++) {
        uint32_t pid;
        int payload_unit_start_indicator;
        byte *adapt, *payload;
        int adapt_len, payload_len;

        err = get_next_TS_packet(tsreader, &pid, &payload_unit_start_indicator, &adapt, &adapt_len,
            &payload, &payload_len);
        if (err == EOF) {
            print_msg("EOF\n");
            break;
        } else if (err) {
            fprint_err("### Error reading TS packet %d\n", ii + 1);
            if (pat_data)
                free(pat_data);
            free_pidint_list(&last_prog_list);
            free_pmt(&last_pmt);
            if (pmt_data)
                free(pmt_data);
            return 1;
        }

        if (pid == 0x0000) {
            num_pats++;
            if (verbose)
                fprint_msg("Packet %d is PAT\n", ii + 1);
            if (payload_len == 0) {
                fprint_msg("Packet %d is PAT, but has no payload\n", ii + 1);
                continue;
            }

            if (payload_unit_start_indicator && pat_data) {
                // This is the start of a new PAT packet, but we'd already
                // started one, so throw its data away
                print_err("!!! Discarding previous (uncompleted) PAT data\n");
                free(pat_data);
                pat_data = nullptr;
                pat_data_len = 0;
                pat_data_used = 0;
            } else if (!payload_unit_start_indicator && !pat_data) {
                // This is the continuation of a PAT packet, but we hadn't
                // started one yet
                print_err("!!! Discarding PAT continuation, no PAT started\n");
                continue;
            }

            err = build_psi_data(
                verbose, payload, payload_len, pid, &pat_data, &pat_data_len, &pat_data_used);
            if (err) {
                fprint_err("### Error %s PAT\n",
                    (payload_unit_start_indicator ? "starting new" : "continuing"));
                if (pat_data)
                    free(pat_data);
                free_pidint_list(&last_prog_list);
                free_pmt(&last_pmt);
                if (pmt_data)
                    free(pmt_data);
                return 1;
            }

            // Do we need more data to complete this PAT?
            if (pat_data_len > pat_data_used)
                continue;

            err = extract_prog_list_from_pat(verbose, pat_data, pat_data_len, &this_prog_list);
            if (err) {
                free_pidint_list(&last_prog_list);
                free_pmt(&last_pmt);
                if (pmt_data)
                    free(pmt_data);
                free(pat_data);
                return err;
            }

            free(pat_data);
            pat_data = nullptr;
            pat_data_len = 0;
            pat_data_used = 0;
            num_pats++;

            if (err) {
                free_pidint_list(&last_prog_list);
                free_pmt(&last_pmt);
                if (pmt_data)
                    free(pmt_data);
                return err;
            }

            if (!same_pidint_list(this_prog_list, last_prog_list)) {
                if (last_prog_list != nullptr)
                    fprint_msg("\nPacket %d is PAT - content changed\n", ii + 1);
                else if (!verbose)
                    fprint_msg("\nPacket %d is PAT\n", ii + 1);

                report_pidint_list(this_prog_list, "Program list", "Program", FALSE);

                if (this_prog_list->length == 0)
                    fprint_msg("No programs defined in PAT (packet %d)\n", ii + 1);
                else {
                    if (this_prog_list->length > 1)
                        fprint_msg("Multiple programs in PAT - using the first\n");
                    pmt_pid = this_prog_list->pid[0];
                }
            }
            free_pidint_list(&last_prog_list);
            last_prog_list = this_prog_list;
        } else if (pid == pmt_pid) {
            if (verbose)
                fprint_msg("Packet %d is PMT with PID %04x (%d)%s\n", ii + 1, pid, pid,
                    (payload_unit_start_indicator ? "[pusi]" : ""));
            if (payload_len == 0) {
                fprint_msg("Packet %d is PMT, but has no payload\n", ii + 1);
                continue;
            }

            if (payload_unit_start_indicator && pmt_data) {
                // This is the start of a new PMT packet, but we'd already
                // started one, so throw its data away
                print_err("!!! Discarding previous (uncompleted) PMT data\n");
                free(pmt_data);
                pmt_data = nullptr;
                pmt_data_len = 0;
                pmt_data_used = 0;
            } else if (!payload_unit_start_indicator && !pmt_data) {
                // This is the continuation of a PMT packet, but we hadn't
                // started one yet
                print_err("!!! Discarding PMT continuation, no PMT started\n");
                continue;
            }

            err = build_psi_data(
                verbose, payload, payload_len, pid, &pmt_data, &pmt_data_len, &pmt_data_used);
            if (err) {
                fprint_err("### Error %s PMT\n",
                    (payload_unit_start_indicator ? "starting new" : "continuing"));
                free_pidint_list(&this_prog_list);
                free_pmt(&last_pmt);
                if (pmt_data)
                    free(pmt_data);
                return 1;
            }

            // Do we need more data to complete this PMT?
            if (pmt_data_len > pmt_data_used)
                continue;

            err = extract_pmt(verbose, pmt_data, pmt_data_len, pid, &this_pmt);
            if (err) {
                free_pidint_list(&this_prog_list);
                free_pmt(&last_pmt);
                if (pmt_data)
                    free(pmt_data);
                return err;
            }

            free(pmt_data);
            pmt_data = nullptr;
            pmt_data_len = 0;
            pmt_data_used = 0;
            num_pmts++;

            if (same_pmt(this_pmt, last_pmt)) // Nothing to do
            {
                free_pmt(&this_pmt);
                continue;
            }

            if (last_pmt != nullptr)
                fprint_msg("\nPacket %d is PMT with PID %04x (%d)"
                           " - content changed\n",
                    ii + 1, pid, pid);
            else if (!verbose)
                fprint_msg("\nPacket %d is PMT with PID %04x (%d)\n", ii + 1, pid, pid);

            report_pmt(TRUE, (char*)"  ", this_pmt);

            free_pmt(&last_pmt);
            last_pmt = this_pmt;
        }
    }

    fprint_msg("\nFound %d PAT packet%s and %d PMT packet%s in %d TS packets\n", num_pats,
        (num_pats == 1 ? "" : "s"), num_pmts, (num_pmts == 1 ? "" : "s"), max);

    free_pidint_list(&last_prog_list);
    free_pmt(&last_pmt);
    if (pmt_data)
        free(pmt_data);
    return 0;
}

void print_usage()
{
    print_msg("Usage: tsinfo [switches] [<infile>]\n"
              "\n");
    REPORT_VERSION("tsinfo");
    print_msg("\n"
              "  Report on the program streams in a Transport Stream.\n"
              "\n"
              "Files:\n"
              "  <infile>  is an H.222 Transport Stream file (but see -stdin)\n"
              "\n"
              "Switches:\n"
              "  -err stdout        Write error messages to standard output (the default)\n"
              "  -err stderr        Write error messages to standard error (Unix traditional)\n"
              "  -stdin             Input from standard input, instead of a file\n"
              "  -verbose, -v       Output extra information about packets\n"
              "  -max <n>, -m <n>   Number of TS packets to scan. Defaults to 10000.\n"
              "  -repeat <n>        Look for <n> PMT packets, and report on each\n");
}

int main(int argc, char** argv)
{
    int use_stdin = FALSE;
    char* input_name = nullptr;
    int had_input_name = FALSE;
    int max = 10000;
    bool verbose = FALSE; // True => output diagnostic/progress messages
    int lookfor = 1;
    int err = 0;

    TS_reader_p tsreader = nullptr;

    int ii = 1;

    if (argc < 2) {
        print_usage();
        return 0;
    }

    while (ii < argc) {
        if (argv[ii][0] == '-') {
            if (!strcmp("--help", argv[ii]) || !strcmp("-h", argv[ii])
                || !strcmp("-help", argv[ii])) {
                print_usage();
                return 0;
            } else if (!strcmp("-err", argv[ii])) {
                MustARG("tsinfo", ii, argc, argv);
                if (!strcmp(argv[ii + 1], "stderr"))
                    redirect_output_stderr();
                else if (!strcmp(argv[ii + 1], "stdout"))
                    redirect_output_stdout();
                else {
                    fprint_err("### tsinfo: "
                               "Unrecognised option '%s' to -err (not 'stdout' or"
                               " 'stderr')\n",
                        argv[ii + 1]);
                    return 1;
                }
                ii++;
            } else if (!strcmp("-verbose", argv[ii]) || !strcmp("-v", argv[ii])) {
                verbose = TRUE;
            } else if (!strcmp("-max", argv[ii]) || !strcmp("-m", argv[ii])) {
                MustARG("tsinfo", ii, argc, argv);
                err = int_value("tsinfo", argv[ii], argv[ii + 1], TRUE, 10, &max);
                if (err)
                    return 1;
                ii++;
            } else if (!strcmp("-repeat", argv[ii])) {
                MustARG("tsinfo", ii, argc, argv);
                err = int_value("tsinfo", argv[ii], argv[ii + 1], TRUE, 10, &lookfor);
                if (err)
                    return 1;
                ii++;
            } else if (!strcmp("-stdin", argv[ii])) {
                use_stdin = TRUE;
                had_input_name = TRUE; // so to speak
            } else {
                fprint_err("### tsinfo: "
                           "Unrecognised command line switch '%s'\n",
                    argv[ii]);
                return 1;
            }
        } else {
            if (had_input_name) {
                fprint_err("### tsinfo: Unexpected '%s'\n", argv[ii]);
                return 1;
            } else {
                input_name = argv[ii];
                had_input_name = TRUE;
            }
        }
        ii++;
    }

    if (!had_input_name) {
        print_err("### tsinfo: No input file specified\n");
        return 1;
    }

    err = open_file_for_TS_read((use_stdin ? nullptr : input_name), &tsreader);
    if (err) {
        fprint_err("### tsinfo: Unable to open input file %s for reading TS\n",
            use_stdin ? "<stdin>" : input_name);
        return 1;
    }
    fprint_msg("Reading from %s\n", (use_stdin ? "<stdin>" : input_name));

    err = report_streams(tsreader, max, verbose);
    if (err) {
        print_err("### tsinfo: Error reporting on stream\n");
        (void)close_TS_reader(&tsreader);
        return 1;
    }

    err = close_TS_reader(&tsreader);
    return (err ? 1 : 0);
}
