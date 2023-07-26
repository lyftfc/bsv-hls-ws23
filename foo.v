module foo (
    input  wire         CLK,
    input  wire         RST,
    input  wire         P,
    input  wire [31:0]  A,
    input  wire [31:0]  B,
    output wire [31:0]  S
);

reg [31:0] Sr;

assign S = Sr;

always @ (posedge CLK or negedge RST) begin
    if (RST)
        Sr <= 32'b0;
    else begin
        if (P)
            Sr <= A + B;
        else
            Sr <= A - B;
    end
end

endmodule