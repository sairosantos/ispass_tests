#!/bin/bash
HOME="/home/srsantos/Experiment"
SIM_HOME=$HOME"/OrCS"
CODE_HOME=$HOME"/ispass_tests/vima_omp"
TRACE_HOME=$HOME"/ispass_tests/vima_omp/traces"
KERNELS=(stencil)
THREADS_N=(8) #4 8 16 32)
DATE_TIME=$(date '+%d%m%Y_%H%M%S');
CACHE_SIZE=(64)

cd $CODE_HOME

if [ ! -d "resultados3" ]; then
	mkdir -p "resultados3"
fi

for KERNEL in "${KERNELS[@]}";
do
	for THREADS in "${THREADS_N[@]}";
	do
		for k in "${CACHE_SIZE[@]}";
		do
			cd $TRACE_HOME
			CONFIG_FILE="configuration_files/vima_variations/sandy_vima_256B_${k}_${THREADS}cores.cfg"
			for i in ${KERNEL}*.${THREADS}t.tid0.stat.out.gz
			do 
    				cd $SIM_HOME
    				TRACE=${i%.tid0.stat.out.gz}
    				COUNTER=0
    				COMMAND="./orcs"
    				while [ $COUNTER -lt $THREADS ]; do
        				COMMAND=${COMMAND}' -t '${TRACE_HOME}/${TRACE}
        			let COUNTER=COUNTER+1
    			done

    			echo "nohup ${COMMAND} -c ${CONFIG_FILE} &> ${CODE_HOME}/resultados3/${TRACE}_${k}_${DATE_TIME}.txt"
        		#nohup ${COMMAND} -c ${CONFIG_FILE} &> ${CODE_HOME}/resultados3/${TRACE}_${k}_${DATE_TIME}.txt &
			done
		done
	done
done
