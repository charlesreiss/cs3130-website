#!/bin/bash
mkdir -p slides
pushd ../cs3130-slides; make
cp _dist/*.pdf ../cs3130-website/slides
popd
