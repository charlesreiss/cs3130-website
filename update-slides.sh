#!/bin/bash
mkdir -p slides
pushd ../cs3130-slides; make
cp _dist/*.pdf ../cs3130-website/slides
popd
pushd ../cs3130-slides/f2024-cr
for i in 2024*; do
    pushd $i
    if [ -e final ] ; then
        make copy-public-final
    else
        make copy-public-heldback
    fi
    popd
done
