open_project -reset eth_accum_hlsprj
set_top eth_accum

add_files eth_accum.cpp
add_files -tb eth_accum_tb.cpp

open_solution au50
set_part xcu50-fsvh2104-2-e
create_clock -period 3 -name clk

# Design Directives
# config_compile -pipeline_style frp

csim_design
csynth_design
# cosim_design -trace_level port_hier

# Bug workaround for IP version number
config_export -version 2.0.1 -vendor nus.edu.sg
export_design -format ip_catalog

exit