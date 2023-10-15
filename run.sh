#!/bin/sh

BLOCK_SIZE=(16 32)
L1_SIZE=(1024)
L1_ASSOC=(1 2)
L2_SIZE=(8192 12288)
L2_ASSOC=(4 6)
PREF_N=(1 3 3 7)
PREF_M=(4 1 4 6)

EN_L2=(0 1)
EN_PF=(0 1)

TRACE_DIR=traces
TRACE=gcc

echo "make all"
make all

for en_pf in ${!EN_PF[@]}; do
	for en_l2 in ${!EN_L2[@]}; do
		for bs in ${!BLOCK_SIZE[@]}; do
			IDX1=$(( bs + en_l2*2 )) 
			IDX2=$(( IDX1 + en_pf*4 )) 
			EN_L2_SIZE=$(( en_l2*L2_SIZE[${bs}] ))
			EN_L2_ASSOC=$(( en_l2*L2_ASSOC[${bs}] ))
			EN_PREF_N=$(( en_pf*PREF_N[${IDX1}] ))
			EN_PREF_M=$(( en_pf*PREF_M[${IDX1}] ))
			echo "./sim ${BLOCK_SIZE[${bs}]} ${L1_SIZE} ${L1_ASSOC[${bs}]} ${EN_L2_SIZE} ${EN_L2_ASSOC} ${EN_PREF_N} ${EN_PREF_M} ${TRACED_DIR}/${TRACE}_trace.txt > output/${TRACE}${IDX2}.txt"
			./sim ${BLOCK_SIZE[${bs}]} ${L1_SIZE} ${L1_ASSOC[${bs}]} ${EN_L2_SIZE} ${EN_L2_ASSOC} ${EN_PREF_N} ${EN_PREF_M} ${TRACED_DIR}/${TRACE}_trace.txt > output/${TRACE}${IDX2}.txt
		done
	done
done

./sim 16 1024 1 0 0 0 0 traces/gcc_trace.txt > output/gcc1.16_1024_1_0_0_0_0.txt
