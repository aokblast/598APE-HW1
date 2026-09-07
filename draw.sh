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

make

if [ "$PIANOROOM" = true ]; then
	sudo perf record -F 99 -g -o ./perf/pianoroom.data -- ./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500
	sudo chmod o+r ./perf/pianoroom.data
	perf script -i ./perf/pianoroom.data > ./perf/pianoroom.perf
	perf/fg/stackcollapse-perf.pl ./perf/pianoroom.perf  > ./perf/pianoroom.folded
	perf/fg/flamegraph.pl ./perf/pianoroom.folded > ./perf/pianoroom.svg
fi

if [ "$GLOBE" = true ]; then
	sudo perf record -F 99 -g -o ./perf/globe.data -- ./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 24
	sudo chmod o+r ./perf/globe.data
	perf script -i ./perf/globe.data > ./perf/globe.perf
	perf/fg/stackcollapse-perf.pl ./perf/globe.perf  > ./perf/globe.folded
	perf/fg/flamegraph.pl ./perf/globe.folded > ./perf/globe.svg
fi

if [ "$ELEPHANT" = true ]; then
	sudo perf record -F 99 -g -o ./perf/elephant.data  -- ./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/sphere.mp4
	sudo chmod o+r ./perf/elephant.data
	perf script -i ./perf/elephant.data > ./perf/elephant.perf
	perf/fg/stackcollapse-perf.pl ./perf/elephant.perf  > ./perf/elephant.folded
	perf/fg/flamegraph.pl ./perf/elephant.folded > ./perf/elephant.svg
fi
