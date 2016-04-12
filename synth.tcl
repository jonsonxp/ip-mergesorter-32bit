############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2015 Xilinx Inc. All rights reserved.
############################################################
open_project ip-mergesorter-32bit
set_top ip_mergesorter_32bit
add_files ./src/main.cpp
add_files ./src/main.hpp
add_files -tb ./src/test.cpp
open_solution "solution1"
set_part {xc7z010clg400-1}
create_clock -period 10 -name default
set_clock_uncertainty 5%
#source "./ip-mergesorter-32bit/solution1/directives.tcl"
csynth_design
#export_design -format syn_dcp
quit
