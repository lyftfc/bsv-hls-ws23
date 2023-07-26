import Vector::*;
import StmtFSM::*;
import FShow::*;
import Cur_Cycle::*;
import VecSum::*;

module mkSimVecSum ();

    let dut <- mkVecSum_32;

    function Action submit_testcase (Vector#(8, UInt#(32)) tvec);
        return (action
            let ts <- cur_cycle;
            $display("[%3d] Submit: ", ts, fshow(tvec));
            dut.submit(tvec);
        endaction);
    endfunction

    function UInt#(32) test_num_plus3 (Integer n);
        return fromInteger(n + 3);
    endfunction

    function UInt#(32) test_num_square (Integer n);
        return fromInteger(n * n);
    endfunction

    function UInt#(32) test_compond (Integer n);
        return test_num_square(n + 3);
    endfunction

    mkAutoFSM (seq
        submit_testcase(genWith(fromInteger));
        submit_testcase(genWith(test_num_plus3));
        submit_testcase(genWith(test_num_square));
        submit_testcase(genWith(test_compond));
        delay(5);
    endseq);

    rule do_get_results;
        let res <- dut.retrieve;
        let ts <- cur_cycle;
        $display("[%3d] Result: %d", ts, res);
    endrule

endmodule

(* synthesize *)
module mkVecSum_32 (VecSum#(32));
    VecSum#(32) m <- mkVecSum;
    return m;
endmodule