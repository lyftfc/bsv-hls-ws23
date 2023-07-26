#include "eth_accum.hpp"
#include <iostream>

int main ()
{
    eth_stream dut_in ("dut_in"), dut_out ("dut_out");
    eth_beat_t curr_beat;

    // Packet format:
    //  B0-13: Ethernet header (no VLAN)
    //  B14-31: Reserved, set to 0
    //  B32-95: 16x uint32 numbers, to be accumulated
    //  B96-103: uint64 accumulated result (to be filled by DUT)
    ap_uint<14*8> ether_header = reverse_bytes(ap_uint<14*8>(
            "01005effffff" "3cecef57087c" "1234", 16));
    // We skip IP header for now
    // Payload (16x uint32, to be added together)
    ap_uint<16*32> test_nums;
    // Counter for generating the test numbers
    ap_uint<32> numgen = 0;

    // Construct the test packets and put into DUT's input FIFO
    // In each loop we send a packet, which has 2 beat (64-byte each)
    for (int i = 0; i < 5; i++) {
        // The first beat: byte 0 to 63
        curr_beat.data(BYTE_RANGE(13, 0)) = ether_header;
        curr_beat.data(BYTE_RANGE(31, 14)) = 0;
        for (int j = 0; j < 8; j++) {
            numgen++;
            curr_beat.data(BYTE_RANGE(35 + 4 * j, 32 + 4 * j)) = reverse_bytes(numgen);
        }
        axiu_keep_bytes(curr_beat);
        curr_beat.last = 0;
        dut_in.write(curr_beat);
        // The second beat: byte 64 to 103 (last, not full)
        for (int j = 0; j < 8; j++){
            numgen++;
            curr_beat.data(BYTE_RANGE(3 + 4 * j, 4 * j)) = reverse_bytes(numgen);
        }
        curr_beat.data(BYTE_RANGE(39, 32)) = 0;
        axiu_keep_bytes(curr_beat, 40);
        curr_beat.last = 1;
        dut_in.write(curr_beat);
    }

    // Running DUT
    while (!dut_in.empty())
        eth_accum(dut_in, dut_out);
    for (int i = 0; i < 20; i++)
        eth_accum(dut_in, dut_out);

    // Read out the results
    while (!dut_out.empty()) {
        // Takes all the last beat (which contains the returned data)
        eth_beat_t beat = dut_out.read();
        if (!beat.last) continue;
        // Extract the data from the packet
        ap_uint<64> result = beat.data(BYTE_RANGE(39, 32));
        result = reverse_bytes(result);
        std::cout << result << std::endl;
    }

    return 0;
}