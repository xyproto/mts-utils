/*
 * A simple test for the ES unit lists from es.c
 *
 */

#include <cmath>
#include <cstdio>
#include <cstdlib>

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

int main(int argc, char** argv)
{
    int err, ii;
    int max = ES_UNIT_LIST_START_SIZE + ES_UNIT_LIST_INCREMENT + 3;
    ES_unit_list_p list = nullptr;
    ES_unit_p unit = nullptr;

    printf("Testing ES unit list\n");
    printf("Test 1 - differing ES units\n");
    err = build_ES_unit_list(&list);
    if (err) {
        printf("Test failed - constructing list\n");
        return 1;
    }
    for (ii = 0; ii < max; ii++) {
        err = build_ES_unit(&unit);
        if (err) {
            printf("Test failed - constructing ES unit\n");
            return 1;
        }
        err = append_to_ES_unit_list(list, unit);
        if (err) {
            printf("Test failed - appending ES unit %d\n", ii);
            return 1;
        }
        if (list->length > list->size) {
            printf("Test failed - list length = %d, size = %d\n", list->length, list->size);
            return 1;
        } else if (list->length != ii + 1) {
            printf("Test failed - list length is %d, expected %d\n", list->length, ii + 1);
            return 1;
        }
        free_ES_unit(&unit);
    }

    printf("Test 1 - resetting list\n");
    reset_ES_unit_list(list);
    if (list->length != 0) {
        printf("Test failed - list length is %d, not 0\n", list->length);
        return 1;
    }

    // And try populating the list again, but a bit further this time
    for (ii = 0; ii < max + ES_UNIT_LIST_INCREMENT; ii++) {
        err = build_ES_unit(&unit);
        if (err) {
            printf("Test failed - constructing ES unit\n");
            return 1;
        }
        err = append_to_ES_unit_list(list, unit);
        if (err) {
            printf("Test failed - appending ES unit %d\n", ii);
            return 1;
        }
        if (list->length > list->size) {
            printf("Test failed - list length = %d, size = %d\n", list->length, list->size);
            return 1;
        } else if (list->length != ii + 1) {
            printf("Test failed - list length is %d, expected %d\n", list->length, ii + 1);
            return 1;
        }
        free_ES_unit(&unit);
    }

    printf("Test 1 - clearing list\n");
    free_ES_unit_list(&list);
    printf("Test 1 succeeded\n");

    printf("Test 2 - the same ES unit inserted multiple times\n");
    err = build_ES_unit_list(&list);
    if (err) {
        printf("Test failed - constructing list\n");
        return 1;
    }
    err = build_ES_unit(&unit);
    if (err) {
        printf("Test failed - constructing ES unit\n");
        return 1;
    }

    // We aren't testing allocation limits this time round
    for (ii = 0; ii < 5; ii++) {
        err = append_to_ES_unit_list(list, unit);
        if (err) {
            printf("Test failed - appending ES unit %d\n", ii);
            return 1;
        }
    }

    printf("Test 2 - clearing list\n");
    free_ES_unit_list(&list);
    free_ES_unit(&unit);
    printf("Test 2 succeeded\n");
    return 0;
}
