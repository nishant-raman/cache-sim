#!/bin/sh

#BLOCK_SIZE=(16 32)
#L1_SIZE=(1024)
#L1_ASSOC=(1 2)
#L2_SIZE=(8192 12288)
#L2_ASSOC=(4 6)
#PREF_N=(1 3 3 7)
#PREF_M=(4 1 4 6)
#
#EN_L2=(0 1)
#EN_PF=(0 1)
#
#TRACE_DIR=traces
#TRACE=gcc
#
#echo "make all"
#make all
#
## validation runs
#for en_pf in ${!EN_PF[@]}; do
#	for en_l2 in ${!EN_L2[@]}; do
#		for bs in ${!BLOCK_SIZE[@]}; do
#			IDX1=$(( bs + en_l2*2 )) 
#			IDX2=$(( IDX1 + en_pf*4 )) 
#			IDX3=$(( IDX2 + 1 )) 
#			EN_L2_SIZE=$(( en_l2*L2_SIZE[${bs}] ))
#			EN_L2_ASSOC=$(( en_l2*L2_ASSOC[${bs}] ))
#			EN_PREF_N=$(( en_pf*PREF_N[${IDX1}] ))
#			EN_PREF_M=$(( en_pf*PREF_M[${IDX1}] ))
#			echo "./sim ${BLOCK_SIZE[${bs}]} ${L1_SIZE} ${L1_ASSOC[${bs}]} ${EN_L2_SIZE} ${EN_L2_ASSOC} ${EN_PREF_N} ${EN_PREF_M} ${TRACED_DIR}/${TRACE}_trace.txt > output/${TRACE}${IDX3}.txt"
#			#./sim ${BLOCK_SIZE[${bs}]} ${L1_SIZE} ${L1_ASSOC[${bs}]} ${EN_L2_SIZE} ${EN_L2_ASSOC} ${EN_PREF_N} ${EN_PREF_M} ${TRACE_DIR}/${TRACE}_trace.txt > output/${TRACE}${IDX3}.txt
#
#			echo gvimdiff val/val${IDX3}.${BLOCK_SIZE[${bs}]}_${L1_SIZE}_${L1_ASSOC[${bs}]}_${EN_L2_SIZE}_${EN_L2_ASSOC}_${EN_PREF_N}_${EN_PREF_M}_${TRACE}.txt output/${TRACE}${IDX3}.txt
#
#		done
#	done
#done
#
## extra runs
#./sim 64 3584 7 0 0 0 0 traces/gcc_trace.txt > output/gcc_e1.txt
#./sim 16 1024 64 0 0 0 0 traces/gcc_trace.txt > output/gcc_e2.txt
#./sim 16 1024 2 0 0 0 0 traces/perl_trace.txt > output/perl_e3.txt
#./sim 32 1024 2 8192 4 0 0 traces/vortex_trace.txt > output/vortex_e4.txt
#./sim 64 8192 4 0 0 8 4 traces/compress_trace.txt > output/compress_e5.txt
#
##report experiment 1
#RE1_BLOCK_SIZE=(32)
#RE1_SIZE=(1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576)
#RE1_ASSOC=(1 2 4 8)
#
#for s in ${RE1_SIZE[@]}; do 
#	for a in ${RE1_ASSOC[@]}; do
#		echo "./sim ${RE1_BLOCK_SIZE} ${s} ${a} 0 0 0 0 traces/gcc_trace.txt > re/gcc1.${RE1_BLOCK_SIZE}_${s}_${a}.txt"
#		./sim ${RE1_BLOCK_SIZE} ${s} ${a} 0 0 0 0 traces/gcc_trace.txt > re/gcc1.${RE1_BLOCK_SIZE}_${s}_${a}_0_0_0_0.txt
#	done
#	FA=$(( s/RE1_BLOCK_SIZE ))
#		echo "./sim ${RE1_BLOCK_SIZE} ${s} ${FA} 0 0 0 0 traces/gcc_trace.txt > re/gcc1.${RE1_BLOCK_SIZE}_${s}_${FA}.txt"
#		./sim ${RE1_BLOCK_SIZE} ${s} ${FA} 0 0 0 0 traces/gcc_trace.txt > re/gcc1.${RE1_BLOCK_SIZE}_${s}_${FA}_0_0_0_0.txt
#done
#
##report experiment 2
#RE2_BLOCK_SIZE=(32)
#RE2_SIZE=(1024 2048 4096 8192)
#RE2_ASSOC=(1 2 4 8)
#
#for s in ${RE2_SIZE[@]}; do 
#	for a in ${RE2_ASSOC[@]}; do
#		echo "./sim ${RE2_BLOCK_SIZE} ${s} ${a} 0 0 0 0 traces/gcc_trace.txt > re/gcc2.${RE2_BLOCK_SIZE}_${s}_${a}_16384_8.txt"
#		./sim ${RE2_BLOCK_SIZE} ${s} ${a} 16384 8 0 0 traces/gcc_trace.txt > re/gcc2.${RE2_BLOCK_SIZE}_${s}_${a}_16384_8_0_0.txt
#	done
#	FA=$(( s/RE2_BLOCK_SIZE ))
#		echo "./sim ${RE2_BLOCK_SIZE} ${s} ${FA} 0 0 0 0 traces/gcc_trace.txt > re/gcc2.${RE2_BLOCK_SIZE}_${s}_${FA}_16384_8.txt"
#		./sim ${RE2_BLOCK_SIZE} ${s} ${FA} 16384 8 0 0 traces/gcc_trace.txt > re/gcc2.${RE2_BLOCK_SIZE}_${s}_${FA}_16384_8_0_0.txt
#done
#
##report experiment 3
#RE3_BLOCK_SIZE=(16 32 64 128)
#RE3_SIZE=(1024 2048 4096 8192 16384 32768)
#RE3_ASSOC=(4)
#
#for bs in ${RE3_BLOCK_SIZE[@]}; do
#	for s in ${RE3_SIZE[@]}; do 
#		for a in ${RE3_ASSOC[@]}; do
#			echo "./sim ${bs} ${s} ${a} 0 0 0 0 traces/gcc_trace.txt > re/gcc3.${bs}_${s}_${a}_0_0.txt"
#			./sim ${bs} ${s} ${a} 0 0 0 0 traces/gcc_trace.txt > re/gcc3.${bs}_${s}_${a}_0_0_0_0.txt
#		done
#	done
#done
#
##report experiment 4
#RE4_BLOCK_SIZE=(32)
#RE4_L1_SIZE=(1024 2048 4096 8192)
#RE4_L1_ASSOC=(4)
#RE4_L2_SIZE=(16384 32768 65536)
#RE4_L2_ASSOC=(8)
#
#for l1 in ${RE4_L1_SIZE[@]}; do 
#	for l2 in ${RE4_L2_SIZE[@]}; do
#			echo "./sim ${RE4_BLOCK_SIZE} ${l1} ${RE4_L1_ASSOC} ${l2} ${RE4_L2_ASSOC} 0 0 traces/gcc_trace.txt > re/gcc4.${RE4_BLOCK_SIZE}_${l1}_${RE4_L1_ASSOC}_${l2}_${RE4_L2_ASSOC}.txt"
#			./sim ${RE4_BLOCK_SIZE} ${l1} ${RE4_L1_ASSOC} ${l2} ${RE4_L2_ASSOC} 0 0 traces/gcc_trace.txt > re/gcc4.${RE4_BLOCK_SIZE}_${l1}_${RE4_L1_ASSOC}_${l2}_${RE4_L2_ASSOC}_0_0.txt
#	done
#done

#report experiment 5
RE5_BLOCK_SIZE=(16 32)
RE5_SIZE=(1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576)
RE5_ASSOC=(1 2 4 8)
#RE5_SIZE=(1024)
#RE5_ASSOC=(1)
PFN=(0 1 2 3 4)
PFM=(4)

for bs in ${RE5_BLOCK_SIZE[@]}; do
	for s in ${RE5_SIZE[@]}; do
		for a in ${RE5_ASSOC[@]}; do
			for pfn in ${PFN[@]}; do
				for pfm in ${PFM[@]}; do 
					echo "./sim ${bs} ${s} ${a} 0 0 ${pfn} ${pfm} microbenchmark/streams_trace.txt > re/gcc5.${bs}_${s}_${a}_0_0_${pfn}_${pfm}.txt"
					./sim ${bs} ${s} ${a} 0 0 ${pfn} ${pfm} microbenchmark/streams_trace.txt > re/gcc5.${bs}_${s}_${a}_0_0_${pfn}_${pfm}.txt
				done
			done
		done
		fa=$(( s/bs ))
		echo "./sim ${bs} ${s} ${fa} 0 0 ${pfn} ${pfm} microbenchmark/streams_trace.txt > re/gcc5.${bs}_${s}_${fa}_0_0_${pfn}_${pfm}.txt"
		./sim ${bs} ${s} ${fa} 0 0 ${pfn} ${pfm} microbenchmark/streams_trace.txt > re/gcc5.${bs}_${s}_${fa}_0_0_${pfn}_${pfm}.txt
	done
done
