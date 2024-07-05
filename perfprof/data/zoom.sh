#!/bin/bash

# Example: ./zoom.sh 1.025 seed_default_tl_120s_nodes_1000_random
# 1.025 -> max asix value
# seed_default_tl_120s_nodes_1000_random -> zoom target

python3 ../perfprof.py -D , -M $1 -T 120 ./matheuristics/$2/$2.csv ./matheuristics/$2/$2\_zoom.pdf -X "Cost Ratio" -P "TSP Performance Profile"