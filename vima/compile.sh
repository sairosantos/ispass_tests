#!/bin/bash
HOME="/home/srsantos/Experiment"
SIM_HOME=$HOME"/OrCS"
PIN_HOME=$SIM_HOME"/trace_generator/pin"
SINUCA_TRACER_HOME=$SIM_HOME"/trace_generator/extras/pinplay/bin/intel64/sinuca_tracer.so"
CODE_HOME=$HOME"/ispass_tests/vima"
EXEC_HOME=$CODE_HOME"/exec"
OUT_HOME=$CODE_HOME"/out"
TRACE_HOME=$CODE_HOME"/traces"
COMP_FLAGS="-O2 -DNOINLINE -static"
VECTORS=(64 128 256) #2048) # 256 8192 1048576)
SIZES=(8 16 32 64)

cd $CODE_HOME

if [ ! -d $EXEC_HOME ]; then
        mkdir -p $EXEC_HOME
fi

if [ ! -d $TRACE_HOME ]; then
	mkdir -p $TRACE_HOME
fi

if [ ! -d $OUT_HOME ]; then
        mkdir -p $OUT_HOME
fi

for i in *tion.cpp
do 
    rm $EXEC_HOME/${i%.cpp}.out
    g++ $i $COMP_FLAGS -o $EXEC_HOME/${i%.cpp}.out
    for j in "${SIZES[@]}";
    do
		for l in "${VECTORS[@]}";
		do
			echo "$PIN_HOME -t $SINUCA_TRACER_HOME -trace iVIM -orcs_tracing 1 -output $TRACE_HOME/${i%.cpp}.${j}MB_${l}B.1t -- $EXEC_HOME/${i%.cpp}.out ${j} ${l} &> $OUT_HOME/${i%.cpp}.${j}MB_${l}B.out &"
			nohup $PIN_HOME -t $SINUCA_TRACER_HOME -trace iVIM -orcs_tracing 1 -output $TRACE_HOME/${i%.cpp}.${j}MB_${l}B.1t -- $EXEC_HOME/${i%.cpp}.out ${j} ${l} &> $OUT_HOME/${i%.cpp}.${j}MB_${l}B.out
		done
    done
done
