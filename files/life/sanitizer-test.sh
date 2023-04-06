#!/bin/bash
set -e
echo "With ThreadSanitizer: input/tiny a for 5 terations"
./life-tsan 5 input/tiny time
echo -----------------------------------
echo "With AddressSanitizer: input/tiny for 5 terations"
./life-asan 5 input/tiny time
echo -----------------------------------

echo "With ThreadSanitizer: input/make-a for 5 terations"
./life-tsan 5 input/make-a time
echo -----------------------------------
echo "With ThreadSanitizer: input/o0075 for 6 iterations"
./life-tsan 6 input/o0075 time
echo -----------------------------------
echo "With ThreadSanitizer: input/o0045-gun for 5 iterations"
./life-tsan 5 input/o0045-gun time
echo -----------------------------------
echo "With ThreadSanitizer: input/puf-qb-c3 for 3 iterations"
./life-tsan 3 input/puf-qb-c3 time
echo -----------------------------------
echo "With ThreadSanitizer: input/23334m for 3 iterations"
./life-tsan 3 input/23334m time
echo -----------------------------------

echo "With AddressSanitizer: input/make-a for 5 terations"
./life-asan 5 input/make-a time
echo -----------------------------------
echo "With AddressSanitizer: input/o0075 for 6 iterations"
./life-asan 6 input/o0075 time
echo -----------------------------------
echo "With AddressSanitizer: input/o0045-gun for 5 iterations"
./life-asan 5 input/o0045-gun time
echo -----------------------------------
echo "With AddressSanitizer: input/puf-qb-c3 for 3 iterations"
./life-asan 3 input/puf-qb-c3 time
echo -----------------------------------
echo "With AddressSanitizer: input/23334m for 3 iterations"
./life-asan 3 input/23334m time
echo -----------------------------------
