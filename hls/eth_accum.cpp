#include "eth_accum.hpp"

// Number array type, contains 8 flattened 32-bit uints
typedef ap_uint <256>               u32_8x_t;
// Result type, 64-bit uint
typedef ap_uint <64>                u64_t;

struct eth_beat_user_t {
    ap_uint<512> data;
    ap_uint<64> keep;
    ap_uint<1> last;
};
typedef hls::stream<eth_beat_user_t> eth_stream_u;

#define ETH_TO_AXIU(x) \
    (eth_beat_t){.data=(x).data,.keep=(x).keep,.strb=(x).keep,.user=0,.last=(x).last,.id=0,.dest=0}
#define ETH_TO_USER(x) \
    (eth_beat_user_t){.data=(x).data,.keep=(x).keep,.last=(x).last}

// The HLS stream abstractions
// These will be translated to AXI4 Streams and AXI-S FIFOs
typedef hls::stream <u32_8x_t>      u32_8x_stream;
typedef hls::stream <u64_t>         u64_stream;

static void num_accum (
    u32_8x_stream   &s_nums,
    u64_stream      &m_accum
) {
    #pragma HLS pipeline ii=1

    static u64_t accum;
    static bool send_res = false;

    ap_uint<33> sum_2x[4];
    ap_uint<34> sum_4x[2];

    if (s_nums.empty()) return;
    u32_8x_t nums = s_nums.read();

    for (int i = 0; i < 4; i++) {
        #pragma HLS unroll
        ap_uint<32> a = nums(BYTE_RANGE(3 + 8 * i, 8 * i));
        ap_uint<32> b = nums(BYTE_RANGE(7 + 8 * i, 4 + 8 * i));
        sum_2x[i] = reverse_bytes(a) + reverse_bytes(b);
    }

    for (int i = 0; i < 2; i++) {
        #pragma HLS unroll
        sum_4x[i] = sum_2x[2 * i] + sum_2x[1 + 2 * i];
    }

    // #pragma HLS bind_op variable=accum op=add impl=dsp latency=1
    accum += sum_4x[0] + sum_4x[1];

    if (send_res) {
        m_accum.write(reverse_bytes(accum));
        send_res = false;
    } else send_res = true;
}

static void pkt_extract (
    eth_stream      &s_eth_rx,
    eth_stream_u    &m_eth_tx,
    u32_8x_stream   &m_nums
) {
    #pragma HLS pipeline ii=1

    static unsigned beat_id = 0;

    if (s_eth_rx.empty()) return;
    eth_beat_t axiu_beat = s_eth_rx.read();
    eth_beat_user_t beat = ETH_TO_USER(axiu_beat);

    if (beat_id == 0)
        m_nums.write(beat.data(BYTE_RANGE(63, 32)));
    else
        m_nums.write(beat.data(BYTE_RANGE(31, 0)));

    if (beat.last)
        beat_id = 0;
    else
        beat_id++;

    m_eth_tx.write(beat);
}

static void pkt_fill (
    eth_stream_u    &s_eth_rx,
    eth_stream      &m_eth_tx,
    u64_stream      &s_accum
) {
    #pragma HLS pipeline ii=1

    static unsigned beat_id = 0;

    if (s_eth_rx.empty()) return;
    if (beat_id == 1 && s_accum.empty()) return;
    eth_beat_user_t beat = s_eth_rx.read();

    if (beat_id == 1)
        beat.data(BYTE_RANGE(39, 32)) = s_accum.read();

    if (beat.last)
        beat_id = 0;
    else
        beat_id++;

    m_eth_tx.write(ETH_TO_AXIU(beat));
}

void eth_accum (
    eth_stream      &s_eth_rx,
    eth_stream      &m_eth_tx
) {
    #pragma HLS dataflow
    #pragma HLS interface axis          port=s_eth_rx
    #pragma HLS interface axis          port=m_eth_tx
    #pragma HLS interface ap_ctrl_none  port=return

    // FIFO for holding the packet beats before we get the result
    static eth_stream_u     preres_fifo     ("preres_fifo");
    #pragma HLS stream      variable=preres_fifo    depth=8
    #pragma HLS data_pack   variable=preres_fifo

    // FIFO for sending numbers to accumulator
    static u32_8x_stream    rxnums_fifo     ("rxnums_fifo");
    #pragma HLS stream      variable=rxnums_fifo    depth=2

    // FIFO for getting accumulator's value back
    static u64_stream       accum_fifo      ("accum_fifo");
    #pragma HLS stream      variable=accum_fifo     depth=2

    pkt_extract (s_eth_rx, preres_fifo, rxnums_fifo);
    num_accum   (rxnums_fifo, accum_fifo);
    pkt_fill    (preres_fifo, m_eth_tx, accum_fifo);

}