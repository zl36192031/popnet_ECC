#!/bin/bash
rm ./throughput_trace/bench.*
python ./throughput_trace/create_trace.py
./popnet -A 9 -c 2 -V 3 -B 12 -O 12 -F 4 -L 1000 -T 80000 -r 1 -I ./throughput_trace/bench -R 1