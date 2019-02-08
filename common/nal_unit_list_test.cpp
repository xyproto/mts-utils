/*
 * A simple test for the NAL unit lists from nalunit.c
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

int main(int argc, char** argv)
{
    int err, ii;
    int max = NAL_UNIT_LIST_START_SIZE + NAL_UNIT_LIST_INCREMENT + 3;
    nal_unit_list_p list = nullptr;
    nal_unit_p unit = nullptr;

    printf("NAL unit lists are now handled as NAL unit lists, so...\n");
    printf("Testing NAL unit list\n");
    printf("Test 1 - differing NAL units\n");
    err = build_nal_unit_list(&list);
    if (err) {
        printf("Test failed - constructing list\n");
        return 1;
    }
    for (ii = 0; ii < max; ii++) {
        err = build_nal_unit(&unit);
        if (err) {
            printf("Test failed - constructing NAL unit\n");
            return 1;
        }
        err = append_to_nal_unit_list(list, unit);
        if (err) {
            printf("Test failed - appending NAL unit %d\n", ii);
            return 1;
        }
        if (list->length > list->size) {
            printf("Test failed - list length = %d, size = %d\n", list->length, list->size);
            return 1;
        } else if (list->length != ii + 1) {
            printf("Test failed - list length is %d, expected %d\n", list->length, ii + 1);
            return 1;
        } else if (list->array[ii] != unit) {
            printf(
                "Test failed - list->array[%d] is %p, expected %p\n", ii, list->array[ii], unit);
            return 1;
        }
    }

    printf("Test 1 - resetting list\n");
    reset_nal_unit_list(list, TRUE);
    if (list->length != 0) {
        printf("Test failed - list length is %d, not 0\n", list->length);
        return 1;
    }
    if (list->array[0] != nullptr) {
        printf("Test failed - list->array[0] is %p, not nullptr\n", list->array[0]);
        return 1;
    }

    // And try populating the list again, but a bit further this time
    for (ii = 0; ii < max + NAL_UNIT_LIST_INCREMENT; ii++) {
        err = build_nal_unit(&unit);
        if (err) {
            printf("Test failed - constructing NAL unit\n");
            return 1;
        }
        err = append_to_nal_unit_list(list, unit);
        if (err) {
            printf("Test failed - appending NAL unit %d\n", ii);
            return 1;
        }
        if (list->length > list->size) {
            printf("Test failed - list length = %d, size = %d\n", list->length, list->size);
            return 1;
        } else if (list->length != ii + 1) {
            printf("Test failed - list length is %d, expected %d\n", list->length, ii + 1);
            return 1;
        } else if (list->array[ii] != unit) {
            printf(
                "Test failed - list->array[%d] is %p, expected %p\n", ii, list->array[ii], unit);
            return 1;
        }
    }

    printf("Test 1 - clearing list\n");
    free_nal_unit_list(&list, TRUE);
    printf("Test 1 succeeded\n");

    printf("Test 2 - the same NAL unit inserted multiple times\n");
    err = build_nal_unit_list(&list);
    if (err) {
        printf("Test failed - constructing list\n");
        return 1;
    }
    err = build_nal_unit(&unit);
    if (err) {
        printf("Test failed - constructing NAL unit\n");
        return 1;
    }

    // We aren't testing allocation limits this time round
    for (ii = 0; ii < 5; ii++) {
        err = append_to_nal_unit_list(list, unit);
        if (err) {
            printf("Test failed - appending NAL unit %d\n", ii);
            return 1;
        }
    }

    printf("Test 2 - clearing list\n");
    free_nal_unit_list(&list, FALSE); // better only do a shallow free
    free_nal_unit(&unit);
    printf("Test 2 succeeded\n");
    return 0;
}
