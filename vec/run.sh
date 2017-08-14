#!/bin/bash

red=`tput setaf 1`
reset=`tput sgr0`

#spec2006 benchmarks

spec2006bench="bzip perl gcc mcf milc namd gobmk soplex povray hmmer libquantum h264 lbm omnet astar sphinx 998.specrand 999.specrand"
spec2017bench="500.perl 502.gcc 505.mcf 508.namd 510.parest 511.povray 520.omnet 523.xalan 525.x264 531.sjeng 519.lbm 538.imagick 541.leela 544.nab 557.xz 999.specrand"
nas="bt sp lu mg ft is cg ep"

cur_dir="$(pwd)"


if [ "$1" == "spec2006" ]; then
    echo "${red}running spec2006 benchmarks${reset}"
    cd spec2006-install
    . ./shrc
    echo $CC
    echo $CXX
    sleep 10
    CC1=$CC CXX1=$CXX runspec --config=mac-gcc.cfg --action=scrub --tune=base all
    for bench in $spec2006bench; do
	CC1=$CC CXX1=$CXX runspec --config=mac-gcc.cfg --action=build --tune=base $bench
    done
    for bench in $spec2006bench; do
	CC1=$CC CXX1=$CXX runspec --config=mac-gcc.cfg --action=run --noreportable --tune=base --size=test $bench
    done
    
elif [ "$1" == "spec2017" ]; then
    echo "${red}running spec2017 benchmarks${reset}"
    cd spec2017-install
    . ./shrc
    CC1=$CC CXX1=$CXX runcpu --config=mac-gcc.cfg --action=scrub --tune=base all
    for bench in $spec2017bench; do
	CC1=$CC CXX1=$CXX runcpu --config=mac-gcc.cfg --action=build --tune=base $bench
    done
    for bench in $spec2017bench; do
	CC1=$CC CXX1=$CXX runcpu --config=mac-gcc.cfg --action=run --noreportable --tune=base --size=test $bench
    done

elif [ "$1" == "nas" ]; then
    echo "${red}running nas benchmarks${reset}"
    cd NPB3.0-omp-C
    mkdir -p bin
    make clean
    for bench in $nas; do
	make $bench CLASS=A
    done
    for bench in $nas; do
	./bin/$bench.A
    done
fi


cd $cur_dir
