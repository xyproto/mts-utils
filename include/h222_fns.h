#pragma once

/*
 * Prototypes for functions relating to H.222 data, whether TS or PS
 *
 */

// We don't include our corresponding header file, since there's
// nothing we depend on from it. However, not that *it* includes
// us, mainly for historical reasons.

const char* h222_stream_type_str(unsigned s);
