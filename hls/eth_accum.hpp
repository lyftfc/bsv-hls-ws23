#pragma once

// High-level Synthesis related types
#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

// The data unit tranferred EACH CYCLE to/from the Ethernet IP
// Its members correspond to wires of the AXI4 Stream interface
//  data        : ap_uint<512>, transferring 64-bytes of data (512b) every cycle
//  keep/strb   : ap_uint<64>, sideband signal for byte-validity
//  last        : ap_uint<1>, sideband signal for last-beat indication in packet mode
typedef ap_axiu <512, 0, 0, 0>      eth_beat_t;

// The HLS stream abstractions
// These will be translated to AXI4 Streams and AXI-S FIFOs
typedef hls::stream <eth_beat_t>    eth_stream;

// Helper macros for getting specific bytes from a wide bus beat
#define BYTE_RANGE(hi, lo)  ((hi) + 1) * 8 - 1, (lo) * 8
#define BYTE(id)            ((id) + 1) * 8 - 1, (id) * 8

// Helper function that converts between endianess
template <int W>
inline ap_uint<W> reverse_bytes(ap_uint<W> data) {
    ap_uint<W> ret;
    int nBytes = W >> 3;
    for (int i = 0; i < nBytes; i++) {
        #pragma HLS unroll
        ret(i * 8 + 7, i * 8) = data((nBytes - i - 1) * 8 + 7, (nBytes - i - 1) * 8);
    }
    return ret;
}

// Helper function that set the keep and strobe bits for an AXI stream beat
template <int W>
inline void axiu_keep_bytes(ap_axiu<W,0,0,0> &axiu, int nbytes = (W + 7) / 8) {
    for (int i = 0; i < (W + 7) / 8; i++) {
        axiu.keep.set_bit(i, i < nbytes);
        axiu.strb.set_bit(i, i < nbytes);
    }
}

// The design's top function
void eth_accum (
    eth_stream      &s_eth_rx,
    eth_stream      &m_eth_tx
);
