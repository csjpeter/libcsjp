#!/bin/bash

if test $# -lt 1; then
	echo "Usage:"
	echo "  $(basename $0) list_of_gnuplot_data_files"
	echo
	echo "  Makes a graph of input data produced by container_speed"
	echo "  test program."
	echo
	exit
fi

while test "x$1" != "x"; do
	NAME=$(basename ${1%.data})
	INPUT_FILE="$1"
	gnuplot << EOF

	set title "${NAME}"

	set key left top
	set grid
	set style data lines;

	set lmargin 12;
	set rmargin 6;
	set bmargin 5;
	set tmargin 4;

	set autoscale x
	set xtics nomirror
	set logscale x
	#set xtics (0,1,4,16,64,256,1024,4096,16384,65536,262144,1048576,4194304,16777216)
	set xlabel "Number of elements in the container"

	set autoscale y
	set logscale y
	#set ytics (0,1,4,16,64)
	set ytics nomirror
	set ylabel "Seconds"

	set output "${NAME}.png"
	set terminal png nocrop noenhanced font verdana 12 size 1600,900
	plot    "${INPUT_FILE}" using 1:2 every 1::0 title columnhead axes x1y1, \
		"${INPUT_FILE}" using 1:3 every 1::0 title columnhead axes x1y1, \
		"${INPUT_FILE}" using 1:4 every 1::0 title columnhead axes x1y1, \
		"${INPUT_FILE}" using 1:5 every 1::0 title columnhead axes x1y1, \
		"${INPUT_FILE}" using 1:6 every 1::0 title columnhead axes x1y1, \
		"${INPUT_FILE}" using 1:7 every 1::0 title columnhead axes x1y1, \
		"${INPUT_FILE}" using 1:8 every 1::0 title columnhead axes x1y1

	set output "${NAME}.pdf"
	set terminal pdf noenhanced mono dashed lw 2 font "Helvetica 12" size 29.7cm,21cm
	replot

#	set output "${NAME}.ps"
#	set terminal postscript landscape noenhanced mono dashed lw 1 "Helvetica, 10" size 29.7cm,21cm
#	replot
EOF
	shift
done
