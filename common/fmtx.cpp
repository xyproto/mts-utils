/*
 * Support for formatting time stamps
 *
 */

#include <cstdio>
#include <cstring>

#include <unistd.h>

#include "compat.h"
#include "fmtx.h"

static TCHAR fmtx_buffers[FMTX_BUFFERS_COUNT][FMTX_BUFFER_SIZE];
static int fmtx_buf_no = 0;

TCHAR* fmtx_alloc()
{
    const int n = fmtx_buf_no++ % FMTX_BUFFERS_COUNT;
    return fmtx_buffers[n];
}

int frac_27MHz(int64_t n) { return (int)((n < 0 ? -n : n) % 300LL); }

const TCHAR* fmtx_timestamp(int64_t n, unsigned int flags)
{
    TCHAR* buf = fmtx_alloc();
    int64_t n27 = n * ((flags & FMTX_TS_N_27MHz) != 0 ? 1LL : 300LL);

    switch (flags & FMTX_TS_DISPLAY_MASK) {
    default:
    case FMTX_TS_DISPLAY_90kHz_RAW:
        _sntprintf(buf, FMTX_BUFFER_SIZE, _T("%") I64FMT _T("dt"), n27 / 300LL);
        break;

    case FMTX_TS_DISPLAY_27MHz_RAW:
        _sntprintf(
            buf, FMTX_BUFFER_SIZE, _T("%") I64FMT _T("d:%03dt"), n27 / 300LL, frac_27MHz(n27));
        break;

    case FMTX_TS_DISPLAY_90kHz_32BIT: {
        int64_t n90 = n27 / 300LL;
        TCHAR* p = buf;
        if (n90 < 0)
            *p++ = _T('-');
        _sntprintf(p, FMTX_BUFFER_SIZE, _T("%ut"), (unsigned int)(n90 < 0 ? -n90 : n90));
        break;
    }

    case FMTX_TS_DISPLAY_ms:
        // No timestamp when converted into ms should exceed 32bits
        _sntprintf(buf, FMTX_BUFFER_SIZE, _T("%dms"), (int)(n27 / 27000LL));
        break;

    case FMTX_TS_DISPLAY_HMS: {
        unsigned int h, m, s, f;
        int64_t a27 = n27 < 0 ? -n27 : n27;
        a27 /= I64K(27); // us
        f = (unsigned int)(a27 % I64K(1000000));
        a27 /= I64K(1000000);
        s = (unsigned int)(a27 % I64K(60));
        a27 /= I64K(60);
        m = (unsigned int)(a27 % I64K(60));
        h = (unsigned int)(a27 / I64K(60));
        _sntprintf(buf, FMTX_BUFFER_SIZE, _T("%s%u:%02u:%02u.%04u"), n27 < 0 ? _T("-") : _T(""), h,
            m, s, f / 1000);
        break;
    }
    }
    return (const TCHAR*)buf;
}

static const struct s2tsfss {
    const char* str;
    int flags;
} s2tsf[] = { { "hms", FMTX_TS_DISPLAY_HMS }, { "ms", FMTX_TS_DISPLAY_ms },
    { "90", FMTX_TS_DISPLAY_90kHz_RAW }, { "32", FMTX_TS_DISPLAY_90kHz_32BIT },
    { "27", FMTX_TS_DISPLAY_27MHz_RAW }, { nullptr, -1 } };

int fmtx_str_to_timestamp_flags(const TCHAR* arg_str)
{
    const struct s2tsfss* p;
    for (p = s2tsf; p->str != nullptr; ++p) {
        if (strcmp(p->str, arg_str) == 0)
            break;
    }
    return p->flags;
}
