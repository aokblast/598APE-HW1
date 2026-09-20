#!/usr/bin/env bash

ELEPHANT=false
PIANOROOM=false
GLOBE=false

usage() {
	echo "./draw.sh -p -g -e"
	echo "-p: Pianoroom"
	echo "-g: Globe"
	echo "-e: Elephant"
}

while getopts "gpe" opt; do
	case "$opt" in
		g)
			GLOBE=true
			;;
		p)
			PIANOROOM=true
			;;
		e)
			ELEPHANT=true
			;;
		\?)
			usage
			exit 1
			;;
	esac
done

if [ ! -d ./perf ]; then
	mkdir ./perf
	git clone https://github.com/brendangregg/FlameGraph.git ./perf/fg
fi

EVT_AVAIL=`sysctl -n kernel.perf_event_paranoid`

if [ ! "$EVT_AVAIL" -eq 1 ]; then
	echo "Impossible to run in such security level. Try sysctl -w kernel.perf_event_paranoid=1"
	exit 1
fi

make

if [ "$PIANOROOM" = true ]; then
	perf record -F max -g -o ./perf/pianoroom.data -- ./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500
	perf script -i ./perf/pianoroom.data > ./perf/pianoroom.perf
	perf/fg/stackcollapse-perf.pl ./perf/pianoroom.perf  > ./perf/pianoroom.folded
	perf/fg/flamegraph.pl ./perf/pianoroom.folded > ./perf/pianoroom.svg
fi

if [ "$GLOBE" = true ]; then
	perf record -F max -g -o ./perf/globe.data -- ./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 24
	perf script -i ./perf/globe.data > ./perf/globe.perf
	perf/fg/stackcollapse-perf.pl ./perf/globe.perf  > ./perf/globe.folded
	perf/fg/flamegraph.pl ./perf/globe.folded > ./perf/globe.svg
fi

if [ "$ELEPHANT" = true ]; then
	perf record -F max -g -o ./perf/elephant.data  -- ./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 24 -W 1000 -H 1000 -o output/sphere.mp4
	perf script -i ./perf/elephant.data > ./perf/elephant.perf
	perf/fg/stackcollapse-perf.pl ./perf/elephant.perf  > ./perf/elephant.folded
	perf/fg/flamegraph.pl ./perf/elephant.folded > ./perf/elephant.svg
fi
