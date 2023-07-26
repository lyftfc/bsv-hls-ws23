NAME := eth_accum

CC := g++
COPTS := -g

ODIR := build
IDIR := . /tools/Xilinx/Vitis_HLS/2021.2/include
DEPS := $(wildcard ./*.hpp)
SRCS := $(wildcard ./*.cpp)
INC_ := $(patsubst %, -I%, $(IDIR))

RPT := $(NAME)_hlsprj/au50/syn/report/$(NAME)_csynth.rpt

all: csim

csim: $(ODIR)/$(NAME)

$(ODIR)/$(NAME): $(SRCS) $(DEPS)
	@mkdir -p $(ODIR)
	$(CC) $(COPTS) $(INC_) -o $@ $(SRCS)

hls:
	@vitis_hls hls.tcl

report: $(RPT)
	@less $<

clean:
	@rm -rf ./build ./.Xil ./*.log ./*_hlsprj

.PHONY: all csim hls clean report