#!/bin/bash

# 1. Put this script in PARSEC 3 benchmark root directory.
# 2. Customize by modifying the following variables.
# 3. Run it.

APPS="blackscholes swaptions"
# Number of threads to run.
NTHR="1 2 4"
# Run each benchmark for how many times.
NTIMES="5"
OUTDIR=data
# Specify input data set.
INPUT=simlarge
# Build before running.
BUILD=true

source env.sh

# build <bench>
build() {
    echo "building $i"
    parsecmgmt -a build -c gcc-hooks -p $1
}

# runbench <bench> <nthr>
run() {
    parsecmgmt -a run -c gcc-hooks -i $INPUT -p $1 -n $2
}

# bench <app>
bench() {
    mkdir -p $OUTDIR
    echo "==============Benchmarking $1=============="
    pushd $OUTDIR
    $BUILD && build $1
    for thr in $NTHR; do
        echo $thr > $1.$thr # print number of threads
        for ((i=1; i<=$NTIMES;i++)); do
            echo "run $1 with $thr threads"
            # print ROI time
            run $1 $thr | awk '/\[HOOKS\] Total time spent in ROI:/ { print substr($7, 0, length($7)-1) }' >> $1.$thr
        done
    done
    paste $1.* > $1
    rm -f $1.*
    popd
}

for a in $APPS; do
    bench $a
done
