#!/usr/bin/env bash

make

piano_room_values=()
globe_values=()
elephant_values=()

NROUNDS="${NROUNDS:-3}"

tmp=$(mktemp)

echo "Start running, will run for ${NROUNDS} rounds"

for ((i=0; i<NROUNDS; i++))
do

echo "Round $i"

echo "Running on Pianoroom:"
./main.exe -i inputs/pianoroom.ray --ppm -o output/pianoroom.ppm -H 500 -W 500 >& $tmp
time=$(cat $tmp | sed -n 's/.*Total time to create images=\([0-9.]*\) seconds.*/\1/p')
echo $time
piano_room_values+=($time)

echo "Running on Globe:"
./main.exe -i inputs/globe.ray --ppm  -a inputs/globe.animate --movie -F 24 >& $tmp
time=$(cat $tmp | sed -n 's/.*Total time to create images=\([0-9.]*\) seconds.*/\1/p')
echo $time
globe_values+=($time)

echo "Running on Elephant:"
./main.exe -i inputs/elephant.ray --ppm  -a inputs/elephant.animate --movie -F 24 -W 100 -H 100 -o output/sphere.mp4  >& $tmp
time=$(cat $tmp | sed -n 's/.*Total time to create images=\([0-9.]*\) seconds.*/\1/p')
echo $time
elephant_values+=($time)

done

average=$(printf "%s\n" "${piano_room_values[@]}" | awk '{sum+=$1} END {print sum/NR}')
echo "PianoRoom Runtime: $average"
average=$(printf "%s\n" "${globe_values[@]}" | awk '{sum+=$1} END {print sum/NR}')
echo "Globe Runtime: $average"
average=$(printf "%s\n" "${elephant_values[@]}" | awk '{sum+=$1} END {print sum/NR}')
echo "Elephant Runtime: $average"
