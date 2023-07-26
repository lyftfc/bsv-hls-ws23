VL_DIR := ./verilog
OBJ_DIR := ./build
TOP_PKG := SimVecSum
TOP_MOD := mk$(TOP_PKG)

VL_SIMER := iverilog
BSLIB_PFX := %/Libraries
BSC_LIBS := $(BSLIB_PFX):$(BSLIB_PFX)/Flute_Addon
BSC_VL := $(VL_DIR):%/Verilog.Vivado:%/Verilog
BSC_FLAGS := -u -bdir $(OBJ_DIR) -info-dir $(OBJ_DIR) -elab -p .:$(VL_DIR):$(BSC_LIBS)

compile: $(VL_DIR)/$(TOP_MOD).v

$(VL_DIR)/$(TOP_MOD).v: ./*.bsv
	@mkdir -p $(VL_DIR)
	@mkdir -p $(OBJ_DIR)
	bsc -verilog $(BSC_FLAGS) -vdir $(VL_DIR) -g $(TOP_MOD) ./$(TOP_PKG).bsv

simulate: $(OBJ_DIR)/$(TOP_MOD)_vsim
	@echo "====== Simulation Output ======"
	@./$< +bscvcd

$(OBJ_DIR)/$(TOP_MOD)_vsim: $(VL_DIR)/$(TOP_MOD).v
	bsc -verilog $(BSC_FLAGS) -vsearch $(BSC_VL) -vsim $(VL_SIMER) -e $(TOP_MOD) -o $@ $<

clean:
	@rm -rf ./build ./verilog *.vcd ./.Xil

.PHONY: clean compile simulate pack_ip
