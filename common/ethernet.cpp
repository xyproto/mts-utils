/* ethernet.c */
/*
 * Routines for taking ethernet packets apart.
 *
 */

#include "ethernet.h"
#include "misc_fns.h"

#include <cstring>

int ethernet_packet_from_pcap(pcaprec_hdr_t* hdr, const uint8_t* data, const uint32_t len,
    ethernet_packet_t* pkt, uint32_t* out_st, uint32_t* out_len)
{
    uint32_t eoh;
    const uint8_t* p = data;
    const uint8_t* const eop = data + len;

    pkt->vlan_count = 0;

    // 14 bytes of src,dest,type ..
    if (len < 14) {
        return ETHERNET_ERR_PKT_TOO_SHORT;
    }

    // PCap doesn't store CRCs - it stores [dst] [src] [type]
    memcpy(pkt->dst_addr, p, 6);
    p += 6;
    memcpy(pkt->src_addr, p, 6);
    p += 6;

    // Type/Length is big-endian.
    pkt->typeorlen = uint_16_be(p);
    p += 2;

    // 0x5DC is the maximum frame length in IEEE 802.3 - anything
    // above that here is a type.
    //
    // Length is just the data length.
    if (pkt->typeorlen <= 0x5DC) {
        (*out_len) = pkt->typeorlen;

        eoh = 14;
    } else {
        // Look for VLAN
        while (pkt->typeorlen == 0x8100) {
            if (pkt->vlan_count >= ETHERNET_VLANS_MAX) {
                return ETHERNET_ERR_TOO_MANY_VLANS;
            }
            if (p + 4 > eop) {
                return ETHERNET_ERR_PKT_TOO_SHORT;
            }

            pkt->vlans[pkt->vlan_count].pcp = (p[0] >> 5) & 7;
            pkt->vlans[pkt->vlan_count].cfi = (p[0] >> 4) & 1;
            pkt->vlans[pkt->vlan_count].vid = uint_16_be(p) & 0xfff;
            p += 2;
            ++pkt->vlan_count;

            pkt->typeorlen = uint_16_be(p);
            p += 2;
        }

        eoh = p - data;

        // pcap doesn't store the checksum or pad ..
        (*out_len) = len - eoh;
    }

    (*out_st) = eoh;

    return 0;
}

/* End file */
