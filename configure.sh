#!/bin/bash

if [ $# -ne 3 ]; then
  echo "usage: ./configuration.sh board clock_period sorter_size [fifo_depth]" 1>&2
  exit 1
fi

ruby ./generator/genIP.rb $3 $4 > ./src/main.cpp
echo "success: Merge sort HLS IP of size $1 is written into ./src/main.cpp" 1>&2

if [ "$1" = "vc707" ]; then
	sed -i "/set_part/c\set_part {xc7vx485tffg1761-2}" synth.tcl
elif [ "$1" = "zybo" ]; then
	sed -i "/set_part/c\set_part {xc7z010clg400-1}" synth.tcl
fi
echo "success: Updated the board FPGA part in synth.tcl" 1>&2

sed -i "/create_clock -period/c\create_clock -period $2 -name default" synth.tcl
echo "success: Updated the clock period in synth.tcl" 1>&2