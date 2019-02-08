#pragma once

/*
 * Support for lists (actually arrays) of PID versus integer
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
#include <cstring>

#include "compat.h"
#include "h222_defns.h"
#include "misc_fns.h"
#include "pidint_fns.h"
#include "printing_fns.h"
#include "ts_fns.h"

// ============================================================================
// PIDINT LIST maintenance
// ============================================================================
/*
 * Initialise a new pid/int list datastructure.
 */
int init_pidint_list(pidint_list_p list)
{
    list->length = 0;
    list->size = PIDINT_LIST_START_SIZE;
    list->number = (int*)malloc(sizeof(int) * PIDINT_LIST_START_SIZE);
    if (list->number == nullptr) {
        print_err("### Unable to allocate array in program list datastructure\n");
        return 1;
    }
    list->pid = (uint32_t*)malloc(sizeof(uint32_t) * PIDINT_LIST_START_SIZE);
    if (list->pid == nullptr) {
        free(list->number);
        print_err("### Unable to allocate array in program list datastructure\n");
        return 1;
    }
    return 0;
}

/*
 * Build a new pid/int list datastructure.
 *
 * Returns 0 if it succeeds, 1 if some error occurs.
 */
int build_pidint_list(pidint_list_p* list)
{
    pidint_list_p new2 = (pidint_list_p)malloc(SIZEOF_PIDINT_LIST);
    if (new2 == nullptr) {
        print_err("### Unable to allocate pid/int list datastructure\n");
        return 1;
    }

    if (init_pidint_list(new2))
        return 1;

    *list = new2;
    return 0;
}

/*
 * Add a pid/integer pair to the end of the list
 *
 * Returns 0 if it succeeds, 1 if some error occurs.
 */
int append_to_pidint_list(pidint_list_p list, uint32_t pid, int program)
{
    if (list == nullptr) {
        print_err("### Unable to append to nullptr pid/int list\n");
        return 1;
    }

    if (list->length == list->size) {
        int newsize = list->size + PIDINT_LIST_INCREMENT;
        list->number = (int*)realloc(list->number, newsize * sizeof(int));
        if (list->number == nullptr) {
            print_err("### Unable to extend pid/int list array\n");
            return 1;
        }
        list->pid = (uint32_t*)realloc(list->pid, newsize * sizeof(uint32_t));
        if (list->pid == nullptr) {
            print_err("### Unable to extend pid/int list array\n");
            return 1;
        }
        list->size = newsize;
    }
    list->number[list->length] = program;
    list->pid[list->length] = pid;
    list->length++;
    return 0;
}

/*
 * Remove a pid/integer pair from the list
 *
 * Returns 0 if it succeeds, 1 if some error occurs.
 */
int remove_from_pidint_list(pidint_list_p list, uint32_t pid)
{
    int index;
    int ii;
    if (list == nullptr) {
        print_err("### Unable to remove entry from nullptr pid/int list\n");
        return 1;
    }

    index = pid_index_in_pidint_list(list, pid);
    if (index == -1) {
        fprint_err("### Cannot remove PID %04x from pid/int list"
                   " - it is not there\n",
            pid);
        return 1;
    }

    for (ii = index; ii < (list->length - 1); ii++) {
        list->pid[ii] = list->pid[ii + 1];
        list->number[ii] = list->number[ii + 1];
    }
    (list->length)--;
    return 0;
}

/*
 * Tidy up and free a pid/int list datastructure after we've finished with it
 *
 * Clears the datastructure, frees it and returns `list` as nullptr.
 *
 * Does nothing if `list` is already nullptr.
 */
void free_pidint_list(pidint_list_p* list)
{
    if (*list == nullptr)
        return;
    if ((*list)->number != nullptr) {
        free((*list)->number);
        (*list)->number = nullptr;
    }
    if ((*list)->pid != nullptr) {
        free((*list)->pid);
        (*list)->pid = nullptr;
    }
    (*list)->length = 0;
    (*list)->size = 0;
    free(*list);
    *list = nullptr;
}

/*
 * Report on a pid/int list's contents
 */
void report_pidint_list(
    pidint_list_p list, const char* list_name, const char* int_name, int pid_first)
{
    if (list == nullptr)
        fprint_msg("%s is nullptr\n", list_name);
    else if (list->length == 0)
        fprint_msg("%s is empty\n", list_name);
    else {
        int ii;
        fprint_msg("%s:\n", list_name);
        for (ii = 0; ii < list->length; ii++) {
            if (pid_first)
                fprint_msg("    PID %04x (%d) -> %s %d\n", list->pid[ii], list->pid[ii], int_name,
                    list->number[ii]);
            else
                fprint_msg("    %s %d -> PID %04x (%d)\n", int_name, list->number[ii],
                    list->pid[ii], list->pid[ii]);
        }
    }
}

/*
 * Lookup a PID to find its index in a pid/int list.
 *
 * Note that if `list` is nullptr, then -1 will be returned - this is to
 * allow the caller to make a query before they have read a list from the
 * bitstream.
 *
 * Returns its index (0 or more) if the PID is in the list, -1 if it is not.
 */
int pid_index_in_pidint_list(pidint_list_p list, uint32_t pid)
{
    int ii;
    if (list == nullptr)
        return -1;
    for (ii = 0; ii < list->length; ii++) {
        if (list->pid[ii] == pid)
            return ii;
    }
    return -1;
}

/*
 * Lookup a PID to find the corresponding integer value in a pid/int list.
 *
 * Returns 0 if the PID is in the list, -1 if it is not.
 */
int pid_int_in_pidint_list(pidint_list_p list, uint32_t pid, int* number)
{
    int ii;
    if (list == nullptr)
        return -1;
    for (ii = 0; ii < list->length; ii++) {
        if (list->pid[ii] == pid) {
            *number = list->number[ii];
            return 0;
        }
    }
    return -1;
}

/*
 * Lookup a PID to see if it is in a pid/int list.
 *
 * Note that if `list` is nullptr, then false will be returned - this is to
 * allow the caller to make a query before they have read a list from the
 * bitstream.
 *
 * Returns true if the PID is in the list, false if it is not.
 */
int pid_in_pidint_list(pidint_list_p list, uint32_t pid)
{
    return pid_index_in_pidint_list(list, pid) != -1;
}

/*
 * Check if two pid/int lists have the same content.
 *
 * Note that:
 *
 *  - a list always compares as the same as itself
 *  - two nullptr lists compare as the same
 *  - the *order* of PID/int pairs in the lists does not matter
 *
 * Returns true if the two have the same content, false otherwise.
 */
int same_pidint_list(pidint_list_p list1, pidint_list_p list2)
{
    int ii;
    if (list1 == list2)
        return true;
    else if (list1 == nullptr || list2 == nullptr)
        return false;
    else if (list1->length != list2->length)
        return false;
    for (ii = 0; ii < list1->length; ii++) {
        uint32_t pid = list1->pid[ii];
        int idx = pid_index_in_pidint_list(list2, pid);
        if (idx == -1)
            return false;
        else if (list1->number[ii] != list2->number[idx])
            return false;
    }
    return true;
}

/*
 * Report on a program stream list (a specialisation of report_pidint_list).
 *
 * - `list` is the stream list to report on
 * - `prefix` is nullptr or a string to put before each line printed
 */
void report_stream_list(pidint_list_p list, char* prefix)
{
    if (prefix != nullptr)
        print_msg(prefix);
    if (list == nullptr)
        print_msg("Program stream list is nullptr\n");
    else if (list->length == 0)
        print_msg("Program stream list is empty\n");
    else {
        int ii;
        print_msg("Program streams:\n");
        for (ii = 0; ii < list->length; ii++) {
            if (prefix != nullptr)
                print_msg(prefix);
            fprint_msg("    PID %04x (%d) -> Stream type %3d (%s)\n", list->pid[ii], list->pid[ii],
                list->number[ii], h222_stream_type_str(list->number[ii]));
        }
    }
}

// ============================================================================
// PMT data maintenance
// ============================================================================
/*
 * Initialise a PMT datastructure's stream lists
 */
static int init_pmt_streams(pmt_p pmt)
{
    pmt->num_streams = 0;
    pmt->streams_size = PMT_STREAMS_START_SIZE;
    pmt->streams = (pmt_stream_p)malloc(SIZEOF_PMT_STREAM * PMT_STREAMS_START_SIZE);
    if (pmt->streams == nullptr) {
        print_err("### Unable to allocate streams in PMT datastructure\n");
        return 1;
    }
    return 0;
}

/*
 * Build a new PMT datastructure.
 *
 * `version_number` should be in the range 0-31, and will be treated as a
 * number modulo 32 if it is not.
 *
 * `PCR_pid` should be a legitimate PCR PID - i.e., in the range 0x0010 to
 * 0x1FFE, or 0x1FFF to indicate "unset". However, for convenience, the
 * value 0 will also be accepted, and converted to 0x1FFF.
 *
 * Returns (a pointer to) the new PMT datastructure, or nullptr if some error
 * occurs.
 */
pmt_p build_pmt(uint16_t program_number, byte version_number, uint32_t PCR_pid)
{
    pmt_p new2;

    if (version_number > 31)
        version_number = version_number % 32;

    if (PCR_pid == 0)
        PCR_pid = 0x1FFF; // unset

    if (PCR_pid != 0x1FFF && (PCR_pid < 0x0010 || PCR_pid > 0x1ffe)) {
        fprint_err("### Error building PMT datastructure\n"
                   "    PCR PID %04x is outside legal program stream range\n",
            PCR_pid);
        return nullptr;
    }

    new2 = (pmt_p)malloc(SIZEOF_PMT);
    if (new2 == nullptr) {
        print_err("### Unable to allocate PMT datastructure\n");
        return nullptr;
    }

    new2->program_number = program_number;
    new2->version_number = version_number;
    new2->PCR_pid = PCR_pid;
    new2->program_info_length = 0;
    new2->program_info = nullptr;

    if (init_pmt_streams(new2)) {
        free(new2);
        return nullptr;
    }

    return new2;
}

/*
 * Set the descriptor data on a PMT. Specifically, 'program info',
 * the descriptor data in the PMT "as a whole".
 *
 * Any previous program information for this PMT is lost.
 *
 * A copy of the program information bytes is taken.
 *
 * Returns 0 if it succeeds, 1 if some error occurs.
 */
int set_pmt_program_info(pmt_p pmt, uint16_t program_info_length, byte* program_info)
{
    if (program_info_length > PMT_MAX_INFO_LENGTH) {
        fprint_err("### Program info length %d is more than %d\n", program_info_length,
            PMT_MAX_INFO_LENGTH);
        return 1;
    }
    if (pmt->program_info == nullptr) {
        pmt->program_info = (byte*)malloc(program_info_length);
        if (pmt->program_info == nullptr) {
            print_err("### Unable to allocate program info in PMT datastructure\n");
            return 1;
        }
    } else if (program_info_length != pmt->program_info_length) {
        // well, we might be shrinking it rather than growing it, but still
        pmt->program_info = (byte*)realloc(pmt->program_info, program_info_length);
        if (pmt->program_info == nullptr) {
            print_err("### Unable to extend program info in PMT datastructure\n");
            return 1;
        }
    }
    memcpy(pmt->program_info, program_info, program_info_length);
    pmt->program_info_length = program_info_length;
    return 0;
}

/*
 * Add a program stream to a PMT datastructure
 *
 * If `ES_info_length` is greater than 0, then `ES_info` is copied.
 *
 * Returns 0 if it succeeds, 1 if some error occurs.
 */
int add_stream_to_pmt(
    pmt_p pmt, uint32_t elementary_PID, byte stream_type, uint16_t ES_info_length, byte* ES_info)
{
    if (pmt == nullptr) {
        print_err("### Unable to append to nullptr PMT datastructure\n");
        return 1;
    }

    if (elementary_PID < 0x0010 || elementary_PID > 0x1ffe) {
        fprint_err("### Error adding stream to PMT\n"
                   "    Elementary PID %04x is outside legal program stream range\n",
            elementary_PID);
        return 1;
    }

    if (ES_info_length > PMT_MAX_INFO_LENGTH) {
        fprint_err("### ES info length %d is more than %d\n", ES_info_length, PMT_MAX_INFO_LENGTH);
        return 1;
    }

    if (pmt->num_streams == pmt->streams_size) {
        int newsize = pmt->streams_size + PMT_STREAMS_INCREMENT;
        pmt->streams = (pmt_stream_p)realloc(pmt->streams, newsize * SIZEOF_PMT_STREAM);
        if (pmt->streams == nullptr) {
            print_err("### Unable to extend PMT streams array\n");
            return 1;
        }
        pmt->streams_size = newsize;
    }
    pmt->streams[pmt->num_streams].stream_type = stream_type;
    pmt->streams[pmt->num_streams].elementary_PID = elementary_PID;
    pmt->streams[pmt->num_streams].ES_info_length = ES_info_length;
    if (ES_info_length > 0) {
        pmt->streams[pmt->num_streams].ES_info = (byte*)malloc(ES_info_length);
        if (pmt->streams[pmt->num_streams].ES_info == nullptr) {
            print_err("### Unable to allocate PMT stream ES info\n");
            return 1;
        }
        memcpy(pmt->streams[pmt->num_streams].ES_info, ES_info, ES_info_length);
    } else
        pmt->streams[pmt->num_streams].ES_info = nullptr;
    pmt->num_streams++;
    return 0;
}

/*
 * Free a PMT stream datastructure
 */
static void free_pmt_stream(pmt_stream_p stream)
{
    if (stream == nullptr)
        return;
    if (stream->ES_info != nullptr) {
        free(stream->ES_info);
        stream->ES_info = nullptr;
    }
}

/*
 * Remove a program stream from a PMT.
 *
 * Returns 0 if it succeeds, 1 if some error occurs.
 */
int remove_stream_from_pmt(pmt_p pmt, uint32_t pid)
{
    int index;
    int ii;
    if (pmt == nullptr) {
        print_err("### Unable to remove entry from nullptr PMT datastructure\n");
        return 1;
    }

    index = pid_index_in_pmt(pmt, pid);
    if (index == -1) {
        fprint_err("### Cannot remove PID %04x from PMT datastructure"
                   " - it is not there\n",
            pid);
        return 1;
    }

    free_pmt_stream(&pmt->streams[index]);

    for (ii = index; ii < (pmt->num_streams - 1); ii++)
        pmt->streams[ii] = pmt->streams[ii + 1];
    (pmt->num_streams)--;
    return 0;
}

/*
 * Tidy up and free a PMT datastructure after we've finished with it
 *
 * Clears the datastructure, frees it and returns `pmt` as nullptr.
 *
 * Does nothing if `pmt` is already nullptr.
 */
void free_pmt(pmt_p* pmt)
{
    if (*pmt == nullptr)
        return;
    if ((*pmt)->num_streams > 0) {
        int ii;
        for (ii = 0; ii < (*pmt)->num_streams; ii++)
            free_pmt_stream(&(*pmt)->streams[ii]);
        (*pmt)->num_streams = 0;
    }
    if ((*pmt)->program_info != nullptr) {
        free((*pmt)->program_info);
        (*pmt)->program_info = nullptr;
    }
    free((*pmt)->streams);
    (*pmt)->program_info_length = 0;
    free(*pmt);
    *pmt = nullptr;
}

/*
 * Lookup a PID to find its index in a PMT datastructure.
 *
 * Note that if `pmt` is nullptr, then -1 will be returned.
 *
 * Returns its index (0 or more) if the PID is in the list, -1 if it is not.
 */
int pid_index_in_pmt(pmt_p pmt, uint32_t pid)
{
    int ii;
    if (pmt == nullptr)
        return -1;
    for (ii = 0; ii < pmt->num_streams; ii++) {
        if (pmt->streams[ii].elementary_PID == pid)
            return ii;
    }
    return -1;
}

/*
 * Lookup a PID to find the corresponding program stream information.
 *
 * Returns a pointer to the stream information if the PID is in the list,
 * nullptr if it is not.
 */
pmt_stream_p pid_stream_in_pmt(pmt_p pmt, uint32_t pid)
{
    int ii;
    if (pmt == nullptr)
        return nullptr;
    for (ii = 0; ii < pmt->num_streams; ii++) {
        if (pmt->streams[ii].elementary_PID == pid)
            return &pmt->streams[ii];
    }
    return nullptr;
}

/*
 * Lookup a PID to see if it is in a PMT datastructure.
 *
 * Note that if `pmt` is nullptr, then false will be returned.
 *
 * Returns true if the PID is in the PMT's stream list, false if it is not.
 */
int pid_in_pmt(pmt_p pmt, uint32_t pid) { return pid_index_in_pmt(pmt, pid) != -1; }

/*
 * Check if two PMT streams have the same content.
 *
 * Returns true if the two have the same content, false otherwise.
 */
static int same_pmt_stream(pmt_stream_p str1, pmt_stream_p str2)
{
    if (str1 == str2) // !!!
        return true;
    else if (str1 == nullptr || str2 == nullptr) // !!!
        return false;
    else if (str1->elementary_PID != str2->elementary_PID)
        return false;
    else if (str1->ES_info_length != str2->ES_info_length)
        return false;
    else if (memcmp(str1->ES_info, str2->ES_info, str1->ES_info_length))
        return false;
    else
        return true;
}

/*
 * Check if two PMT datastructures have the same content.
 *
 * Note that:
 *
 *  - a PMT datastructure always compares as the same as itself
 *  - two nullptr datastructures compare as the same
 *  - a different version number means a different PMT
 *  - the *order* of program streams in the PMTs does not matter
 *  - descriptors must be identical as well, and byte order therein
 *    does matter (this may need changing later on)
 *
 * Returns true if the two have the same content, false otherwise.
 */
int same_pmt(pmt_p pmt1, pmt_p pmt2)
{
    int ii;
    if (pmt1 == pmt2)
        return true;
    else if (pmt1 == nullptr || pmt2 == nullptr)
        return false;
    else if (pmt1->PCR_pid != pmt2->PCR_pid)
        return false;
    else if (pmt1->version_number != pmt2->version_number)
        return false;
    else if (pmt1->program_info_length != pmt2->program_info_length)
        return false;
    else if (pmt1->num_streams != pmt2->num_streams)
        return false;
    else if (memcmp(pmt1->program_info, pmt2->program_info, pmt1->program_info_length))
        return false;

    for (ii = 0; ii < pmt1->num_streams; ii++) {
        uint32_t pid = pmt1->streams[ii].elementary_PID;
        int idx = pid_index_in_pmt(pmt2, pid);
        if (idx == -1)
            return false;
        else if (!same_pmt_stream(&pmt1->streams[ii], &pmt2->streams[idx]))
            return false;
    }
    return true;
}

/*
 * Report on a PMT datastructure.
 *
 * - if `is_msg`, report as a message, otherwise as an error
 * - `prefix` is nullptr or a string to put before each line printed
 * - `pmt` is the PMT to report on
 */
void report_pmt(int is_msg, char* prefix, pmt_p pmt)
{
    if (prefix != nullptr)
        fprint_msg_or_err(is_msg, prefix);
    if (pmt == nullptr) {
        fprint_msg_or_err(is_msg, "PMT is nullptr\n");
        return;
    } else
        fprint_msg_or_err(is_msg, "Program %d, version %d, PCR PID %04x (%d)\n",
            pmt->program_number, pmt->version_number, pmt->PCR_pid, pmt->PCR_pid);

    if (pmt->program_info_length > 0) {
        if (prefix != nullptr)
            fprint_msg_or_err(is_msg, prefix);
        print_data(is_msg, "   Program info", pmt->program_info, pmt->program_info_length,
            pmt->program_info_length);
        print_descriptors(is_msg, prefix, "   ", pmt->program_info, pmt->program_info_length);
    }
    if (pmt->num_streams > 0) {
        int ii;
        if (prefix != nullptr)
            fprint_msg_or_err(is_msg, prefix);
        fprint_msg_or_err(is_msg, "Program streams:\n");
        for (ii = 0; ii < pmt->num_streams; ii++) {
            if (prefix != nullptr)
                fprint_msg_or_err(is_msg, prefix);
            fprint_msg_or_err(is_msg, "  PID %04x (%4d) -> Stream type %02x (%3d) %s\n",
                pmt->streams[ii].elementary_PID, pmt->streams[ii].elementary_PID,
                pmt->streams[ii].stream_type, pmt->streams[ii].stream_type,
                h222_stream_type_str(pmt->streams[ii].stream_type));
            if (pmt->streams[ii].ES_info_length > 0) {
                if (prefix != nullptr)
                    fprint_msg_or_err(is_msg, prefix);
                print_data(is_msg, "      ES info", pmt->streams[ii].ES_info,
                    pmt->streams[ii].ES_info_length, pmt->streams[ii].ES_info_length);
                print_descriptors(is_msg, prefix, "      ", pmt->streams[ii].ES_info,
                    pmt->streams[ii].ES_info_length);
            }
        }
    }
}
