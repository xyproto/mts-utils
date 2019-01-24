#pragma once
/*
 * Routines for taking ethernet packets apart.
 *
 */

#include <cstdio>
#include <cstdlib>

#include "compat.h"
#include "pcap.h"

typedef struct ethernet_vlan_info_s {
    char cfi;
    char pcp;
    int vid;
} ethernet_vlan_info_t;

typedef struct ethernet_packet_s {
    // Source address.
    uint8_t src_addr[6];

    // Destination ddress.
    uint8_t dst_addr[6];

    // 0x5DC is the maximum frame length in 802.3
#define ETHERNET_MAY_BE_IP(typeorlen) ((typeorlen) == 0x800 || (typeorlen) <= 0x5DC)

#define ETHERNET_TYPE_IP 0x800
    // Type
    uint16_t typeorlen;

#define ETHERNET_VLANS_MAX 2
    int vlan_count;
    ethernet_vlan_info_t vlans[ETHERNET_VLANS_MAX];

    // Checksum if present. Note that pcap doesn't include checksums.
    uint32_t checksum;
} ethernet_packet_t;

#define ETHERNET_ERR_PKT_TOO_SHORT (-1)
#define ETHERNET_ERR_TOO_MANY_VLANS (-2)

/*
 * \param out_st OUT Index into data at which ethernet payload starts.
 * \param out_len OUT Length of the ethernet payload.
 * \return 0 on success, -1 on failure.
 *

 */
int ethernet_packet_from_pcap(pcaprec_hdr_t* hdr, const uint8_t* data, const uint32_t len,
    ethernet_packet_t* pkt, uint32_t* out_st, uint32_t* out_len);
