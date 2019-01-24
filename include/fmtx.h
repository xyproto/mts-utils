#pragma once

/*
 * Support for formatting time stamps
 *
 */

#define FMTX_BUFFERS_COUNT 8
#define FMTX_BUFFER_SIZE 128

typedef char TCHAR;
#define _T(x) x
#define I64FMT "ll"
#define I64K(x) x##LL
#define _stprintf sprintf
#define _sntprintf snprintf
#define _tcscmp strcmp

// Flags to fmtx_time_stamp
#define FMTX_TS_N_90kHz 0 // Supplied time stamp is in 90kHz units
#define FMTX_TS_N_27MHz 1 // Supplied time stamp is in 27Mhz units

#define FMTX_TS_DISPLAY_MASK 0xff0
#define FMTX_TS_DISPLAY_90kHz_RAW 0
#define FMTX_TS_DISPLAY_90kHz_32BIT 0x10
#define FMTX_TS_DISPLAY_27MHz_RAW 0x20
#define FMTX_TS_DISPLAY_ms 0x30
#define FMTX_TS_DISPLAY_HMS 0x40

const TCHAR* fmtx_timestamp(int64_t n, unsigned int flags);
int fmtx_str_to_timestamp_flags(const TCHAR* arg_str);
