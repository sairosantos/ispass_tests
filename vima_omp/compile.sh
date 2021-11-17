#!/bin/bash
HOME="/home/srsantos/Experiment"
SIM_HOME=$HOME"/OrCS"
PIN_HOME=$SIM_HOME"/trace_generator/pin"
SINUCA_TRACER_HOME=$SIM_HOME"/trace_generator/extras/pinplay/bin/intel64/sinuca_tracer.so"
CODE_HOME=$HOME"/ispass_tests/vima_omp"
EXEC_HOME=$CODE_HOME"/exec"
OUT_HOME=$CODE_HOME"/out"
TRACE_HOME=$CODE_HOME"/traces"
COMP_FLAGS="-O2 -DNOINLINE -mavx2 -march=native -fopenmp"
SIZES=(1) #2 4 8 16 32 64)
THREADS_N=(2) #4 8 16 32)

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

for THREADS in "${THREADS_N[@]}";
do
	for i in *.cpp
	do 
    	rm exec/${i%.cpp}.out
    	g++ $i $COMP_FLAGS -o exec/${i%.cpp}.out
    	export OMP_NUM_THREADS=${THREADS}
		export OMP_WAIT_POLICY=passive
    	for j in "${SIZES[@]}";
		do
			echo "$PIN_HOME -t $SINUCA_TRACER_HOME -orcs_tracing 1 -trace iVIM -output $TRACE_HOME/${i%.cpp}.${j}MB.${THREADS}t -threads ${THREADS} -- $EXEC_HOME/${i%.cpp}.out ${j} &> $OUT_HOME/${i%.cpp}.${j}MB.out"
			nohup $PIN_HOME -t $SINUCA_TRACER_HOME -orcs_tracing 1 -trace iVIM -output $TRACE_HOME/${i%.cpp}.${j}MB.${THREADS}t -threads ${THREADS} -- $EXEC_HOME/${i%.cpp}.out ${j} &> $OUT_HOME/${i%.cpp}.${j}MB.out
		done
	done
done
