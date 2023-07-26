package VecSum;

import FIFO::*;
import Vector::*;

interface VecSum #(numeric type nw);
    method Action submit (Vector#(8, UInt#(nw)) nums);
    method ActionValue#(UInt#(TAdd#(nw, 3))) retrieve ();
endinterface

module mkVecSum (VecSum#(nw)) provisos (Add#(_nw0, 1, nw));

    FIFO#(Vector#(4, UInt#(TAdd#(nw, 1)))) r1 <- mkLFIFO;
    FIFO#(Vector#(2, UInt#(TAdd#(nw, 2)))) r2 <- mkLFIFO;
    FIFO#(UInt#(TAdd#(nw, 3))) qres <- mkSizedFIFO(8);

    rule do_reduce_1;
        let r1vec = r1.first;
        r1.deq;
        Vector#(2, UInt#(TAdd#(nw, 2))) r2vec;
        for (Integer i = 0; i < 2; i = i + 1)
            r2vec[i] = extend(r1vec[i * 2]) + extend(r1vec[i * 2 + 1]);
        r2.enq(r2vec);
    endrule

    rule do_reduce_2;
        let r2vec = r2.first;
        r2.deq;
        UInt#(TAdd#(nw, 3)) res = extend(r2vec[0]) + extend(r2vec[1]);
        qres.enq(res);
    endrule

    method Action submit (Vector#(8, UInt#(nw)) nums);
        Vector#(4, UInt#(TAdd#(nw, 1))) r1vec;
        for (Integer i = 0; i < 4; i = i + 1)
            r1vec[i] = extend(nums[i * 2]) + extend(nums[i * 2 + 1]);
        r1.enq(r1vec);
    endmethod

    method ActionValue#(UInt#(TAdd#(nw, 3))) retrieve ();
        let res = qres.first;
        qres.deq;
        return res;
    endmethod

endmodule : mkVecSum

endpackage : VecSum