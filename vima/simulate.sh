#!/bin/bash
HOME="/home/srsantos/Experiment"
SIM_HOME=$HOME"/OrCS"
CODE_HOME=$HOME"/ispass_tests/vima"
TRACE_HOME=$CODE_HOME"/traces"
VECTOR_SIZE=(8K)
CACHE_SIZE=(64) #128 256 512)
DATE_TIME=$(date '+%d%m%Y_%H%M%S');

cd $CODE_HOME

if [ ! -d "resultados3" ]; then
	mkdir -p "resultados3"
fi

cd $TRACE_HOME
for i in *unrolled.*_64B*.tid0.stat.out.gz
do 
    cd $SIM_HOME
    for j in "${CACHE_SIZE[@]}";
    do
        TRACE=${i%.tid0.stat.out.gz}
        echo "./orcs -t ${TRACE_HOME}/${TRACE} -c configuration_files/vima_variations/sandy_vima_256B_${j}_1cores.cfg &> ${CODE_HOME}/resultados/${TRACE}_256B_${j}_1cores_${DATE_TIME}.txt &"
        nohup ./orcs -t ${TRACE_HOME}/${TRACE} -c configuration_files/vima_variations/sandy_vima_256B_${j}_1cores.cfg &> ${CODE_HOME}/resultados3/${TRACE}_256B_${j}_${DATE_TIME}.txt 
    done
done

cd $TRACE_HOME
for i in *.*_2048B*.tid0.stat.out.gz
do 
    cd $SIM_HOME
    for j in "${CACHE_SIZE[@]}";
    do
        TRACE=${i%.tid0.stat.out.gz}
        #echo "./orcs -t ${TRACE_HOME}/${TRACE} -c configuration_files/vima_variations/sandy_vima_8K_${j}_1cores.cfg &> ${CODE_HOME}/resultados/${TRACE}_8K_${j}_1cores_${DATE_TIME}.txt &"
        #nohup ./orcs -t ${TRACE_HOME}/${TRACE} -c configuration_files/vima_variations/sandy_vima_8K_${j}_1cores.cfg &> ${CODE_HOME}/resultados3/${TRACE}_8K_${j}_${DATE_TIME}.txt &
    done
done
