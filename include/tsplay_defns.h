#pragma once

/*
 * Support for playing (streaming) TS packets.
 *
 * Exposes the functionality in tsplay_innards.c, mainly for use by tsplay.c
 *
 */

// If not being quiet, report progress every TSPLAY_REPORT_EVERY packets read
#define TSPLAY_REPORT_EVERY 10000

typedef enum tsplay_output_pace_mode_e {
    TSPLAY_OUTPUT_PACE_FIXED,
    TSPLAY_OUTPUT_PACE_PCR1, // Src buffering timing
    TSPLAY_OUTPUT_PACE_PCR2_TS, // write buffer timing - use 1st PCR found for PID
    TSPLAY_OUTPUT_PACE_PCR2_PMT // write buffer timing = look up PCR PID in PMT
} tsplay_output_pace_mode;
