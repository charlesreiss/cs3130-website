#!/bin/bash
set -e
echo "input/make-a for 1000 terations"
./life 1000 input/make-a time
echo -----------------------------------
echo "input/o0075 for 1001 iterations"
./life 1001 input/o0075 time
echo -----------------------------------
echo "input/o0045-gun for 1000 iterations"
./life 1000 input/o0045-gun time
echo -----------------------------------
echo "input/puf-qb-c3 for 51 iterations"
./life 51 input/puf-qb-c3 time
echo -----------------------------------
echo "input/23334m for 10 iterations"
./life 10 input/23334m time
echo -----------------------------------
