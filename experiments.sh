#!/bin/bash

bn=5
sn=10
en=100
iter_count=3
sample_count=10
PARAMETERS="-begin_n $bn -end_n $en -step_n $sn -iter_count $iter_count -sample_count $sample_count -detailed True"

BENCHMARK_EXECUTABLE=./build/src/benchmarkAgelatency
for kind in harmonic automotive generic; do
    $BENCHMARK_EXECUTABLE  -logfile $kind.csv -kind ${kind} $PARAMETERS
    $BENCHMARK_EXECUTABLE -DiEqualTi  -logfile ${kind}diti.csv -kind ${kind} $PARAMETERS
done
