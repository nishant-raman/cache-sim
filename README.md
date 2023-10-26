# cache-sim
ECE 563 Project1 - Cache and Memory Hierarchy Simulator

## Functionalities
1. L1 and L2
2. Config size, block-size, assoc
3. Stream buffer 
4. configurable SB (N,M)

## Execution and Inputs

1. `make all` to compile
2. `./sim <block-size> <L1-size> <L1-assoc> <L2-size> <L2-assoc> <N> <M> <trace-file>`

Input constraints:
1. block-size and no. of sets is power of 2
2. l2-size 0 to disable l2
3. N=0 to disable prefetching
4. non zero values of enabled units

Trace file format:

`<r|w> <hex-address>`

## Outputs

1. Final L1 cache contents
2. Final L2 cache contents (if enabled)
3. Final Stream-buffer contents (if enabled)
4. Statistics of cache and memory accesses

## Debugging

DBG Macro to enable debug print statements
Prints cache and SB contents for each request in the trace file

## Scripts

For the purpose of running multiple expirements to compare different configurations of cache
a `run.sh` script has been prepared that will generate several output file in the `re/` directory.
These can be then parsed using `parse.py` script that will save relevant statistics in the `re.csv` file.

Make targets have been created for these and each file may be modified as per personal use-case:
`make csv`
`make exp`
`make parse`
