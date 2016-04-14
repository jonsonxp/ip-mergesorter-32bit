#!/bin/bash
PROGNAME=$(basename $0)
VERSION="0.0.1"

usage() {
  echo "usage: ./configure.sh" 1>&2
  echo "Options:" 1>&2
  echo "    -shell NAME           : the shell name." 1>&2
  echo "    -size NUMBER          : the input size of the merge tree." 1>&2
  echo "    [-fifo_depth NUMBER]  : the input fifo depth of the merge tree." 1>&2
  echo "    -clk_period NUMBER    : the ms period of clock." 1>&2
  exit 1
}

for i in "$@"
do
case $i in
	    '-h'|'--help' )
            usage
            exit 1
        ;;
        '--version' )
            echo $VERSION
            exit 1
            ;;
        -shell)
        	if [ -z "$2" ]; then
        		echo "option requires an argument -- $1" 1>&2
        		exit 1
        	fi
        	SHELL=$2
        	shift 2
        	;;
        -size)
        	if [ -z "$2" ]; then
        		echo "option requires an argument -- $1" 1>&2
        		exit 1
        	fi
        	SIZE=$2
        	shift 2
        	;;
        -clk_period)
        	if [ -z "$2" ]; then
        		echo "option requires an argument -- $1" 1>&2
        		exit 1
        	fi
        	CLK=$2
        	shift 2
        	;;        
        -fifo_depth)
        	if [ -z "$2" ]; then
        		FIFO=1000
        	else
        		FIFO=$2
        	fi
        	shift 2
        	;;
        -*)
		shift 2
        ;;
        *)
        ;;
esac
done

if [ -z $SHELL ]; then
    echo "$PROGNAME: too few arguments" 1>&2
    echo "Try '$PROGNAME --help' for more information." 1>&2
    exit 1
fi
if [ -z $SIZE ]; then
    echo "$PROGNAME: too few arguments" 1>&2
    echo "Try '$PROGNAME --help' for more information." 1>&2
    exit 1
fi
if [ -z $CLK ]; then
    echo "$PROGNAME: too few arguments" 1>&2
    echo "Try '$PROGNAME --help' for more information." 1>&2
    exit 1
fi

#main.cpp
ruby ./generator/genIP.rb $SIZE $FIFO > ./src/main.cpp
echo "success: Merge sort HLS IP of size $1 is written into ./src/main.cpp" 1>&2

sed -i "/define LOOPSIZE/c\#define LOOPSIZE $SIZE" ./driver/rt_mergetree/java/util/sortlib.c

#synth.tcl set_part
if [ "$SHELL" = "shell-vc707-xillybus-ap_fifo32" ]; then
	sed -i "/set_part/c\set_part {xc7vx485tffg1761-2}" synth.tcl
elif [ "$SHELL" = "shell-zybo-xillybus-ap_fifo32" ]; then
	sed -i "/set_part/c\set_part {xc7z010clg400-1}" synth.tcl
else
	echo "Unknown shell: $SHELL" 1>&2
    echo "Try '$PROGNAME --help' for more information." 1>&2
    exit 1
fi
echo "success: Updated the board FPGA part in synth.tcl" 1>&2

#synth.tcl clk
sed -i "/create_clock -period/c\create_clock -period $CLK -name default" synth.tcl
echo "success: Updated the clock period in synth.tcl" 1>&2
