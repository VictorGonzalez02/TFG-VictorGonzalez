#!/bin/bash

duration=60
output="fps.log"

/home/victor/TFG-VictorGonzalez/build/P2-GPUToy > $output &
pid=$!

sleep $duration

kill $pid

sum=$(awk '{sum+=$1} END {print sum}' $output)
count=$(wc -l < $output)
avg=$(echo "$sum / $count" | bc -l)

echo "Average FPS over $duration seconds: $avg"
