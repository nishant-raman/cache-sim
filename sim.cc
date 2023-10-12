#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sim.h"

using namespace std;

/*  "argc" holds the number of command-line arguments.
    "argv[]" holds the arguments themselves.

    Example:
    ./sim 32 8192 4 262144 8 3 10 gcc_trace.txt
    argc = 9
    argv[0] = "./sim"
    argv[1] = "32"
    argv[2] = "8192"
    ... and so on
*/
int main (int argc, char *argv[]) {

	FILE *fp;			// File pointer.
	char *trace_file;		// This variable holds the trace file name.
	cache_params_t params;	// Look at the sim.h header file for the definition of struct cache_params_t.
	char rw;			// This variable holds the request's type (read or write) obtained from the trace.
	uint32_t addr;		// This variable holds the request's address obtained from the trace.
 			// The header file <inttypes.h> above defines signed and unsigned integers of various sizes in a machine-agnostic way.  "uint32_t" is an unsigned integer of 32 bits.

	// Exit with an error if the number of command-line arguments is incorrect.
	if (argc != 9) {
	   printf("Error: Expected 8 command-line arguments but was provided %d.\n", (argc - 1));
	   exit(EXIT_FAILURE);
	}
 
	// "atoi()" (included by <stdlib.h>) converts a string (char *) to an integer (int).
	params.BLOCKSIZE = (uint32_t) atoi(argv[1]);
	params.L1_SIZE   = (uint32_t) atoi(argv[2]);
	params.L1_ASSOC  = (uint32_t) atoi(argv[3]);
	params.L2_SIZE   = (uint32_t) atoi(argv[4]);
	params.L2_ASSOC  = (uint32_t) atoi(argv[5]);
	params.PREF_N    = (uint32_t) atoi(argv[6]);
	params.PREF_M    = (uint32_t) atoi(argv[7]);
	trace_file       = argv[8];
	
	// Enables for L2 cache, prefetch units
	bool en_l2 = (params.L2_SIZE != 0);
	bool en_prefetch = (params.PREF_N != 0);
	bool en_prefetch_l1 = !en_l2 && en_prefetch;
	bool en_prefetch_l2 = en_l2 && en_prefetch;

	// Instantiating Cache Objects
	Cache* l1 = new Cache(params.BLOCKSIZE, params.L1_SIZE, params.L1_ASSOC, en_prefetch_l1, params.PREF_N, params.PREF_M);
	Cache* l2 = NULL;
	if (en_l2) {
		l2 = new Cache(params.BLOCKSIZE, params.L2_SIZE, params.L2_ASSOC, en_prefetch_l2, params.PREF_N, params.PREF_M);
		l2->setnext(NULL);
	}
	l1->setnext(l2);
	
	Metrics main_mem_metrics;
	Metrics* mmm = &main_mem_metrics;
	if (en_l2) {
		l2->setmmm(mmm);
	} else {
		l1->setmmm(mmm);
	}

	// Open the trace file for reading.
	fp = fopen(trace_file, "r");
	if (fp == (FILE *) NULL) {
	   // Exit with an error if file open failed.
	   printf("Error: Unable to open file %s\n", trace_file);
	   exit(EXIT_FAILURE);
	}
 
	// Print simulator configuration.
	printf("===== Simulator configuration =====\n");
	printf("BLOCKSIZE:  %u\n", params.BLOCKSIZE);
	printf("L1_SIZE:    %u\n", params.L1_SIZE);
	printf("L1_ASSOC:   %u\n", params.L1_ASSOC);
	printf("L2_SIZE:    %u\n", params.L2_SIZE);
	printf("L2_ASSOC:   %u\n", params.L2_ASSOC);
	printf("PREF_N:     %u\n", params.PREF_N);
	printf("PREF_M:     %u\n", params.PREF_M);
	printf("trace_file: %s\n", trace_file);
	//printf("===================================\n");

	printf("\n");
	
	//int i = 0;
	// Read requests from the trace file and echo them back.
	while (fscanf(fp, "%c %x\n", &rw, &addr) == 2) {	// Stay in the loop if fscanf() successfully parsed two tokens as specified.
		/*
		if (rw == 'r')
			printf("r %x\n", addr);
		else if (rw == 'w')
			printf("w %x\n", addr);
		else {
			printf("Error: Unknown request type %c.\n", rw);
			exit(EXIT_FAILURE);
		}
		*/
		//DEBUG BEGIN
		//i++;
		//if (rw == 'r')
		//	printf("%d=r %x\n", i,addr);
		//else {
		//	printf("%d=w %x\n", i,addr);
		//}
		//DEBUG END

	///////////////////////////////////////////////////////
	// Issue the request to the L1 cache instance here.
	///////////////////////////////////////////////////////
		l1->request(rw, addr);

	}
	
	// Print results 
	print_results(l1, l2, mmm);

	// Release memory
	delete l1;
	if (en_l2) {
		delete l2;
	}

	return(0);

}


// --------------------- Metrics functions --------------------------
// Constructor for Metrics
Metrics::Metrics() : n_acc(0), n_rd(0), n_rd_miss(0), n_wr(0), n_wr_miss(0), n_wb(0) {
	// DEBUG BEGIN
	//printf("Metrics construction\n");
	//printf("n_acc %d, n_rd %d, n_wr %d, n_rd_miss %d, n_wr_miss %d\n", n_acc, n_rd, n_wr, n_rd_miss, n_wr_miss);
	// DEBUG END
}

// Setters
// Increment number of accesses of current cache
void Metrics::incr_acc () {
	n_acc++;
	return;
}

// Increment number of reads of current cache
void Metrics::incr_rd () {
	n_rd++;
	return;
}

// Increment number of read misses of current cache
void Metrics::incr_rdmiss () {
	n_rd_miss++;
	return;
}

// Increment number of writes of current cache
void Metrics::incr_wr () {
	n_wr++;
	return;
}

// Increment number of write misses of current cache
void Metrics::incr_wrmiss () {
	n_wr_miss++;
	return;
}

// Increment number of writebacks of current cache
void Metrics::incr_wb () {
	n_wb++;
	return;
}

// Getters
uint32_t Metrics::get_acc () {
	return n_acc;
}

uint32_t Metrics::get_rd () {
	return n_rd;
}

uint32_t Metrics::get_rdmiss () {
	return n_rd_miss;
}

uint32_t Metrics::get_wr () {
	return n_wr;
}

uint32_t Metrics::get_wrmiss () {
	return n_wr_miss;
}

uint32_t Metrics::get_wb () {
	return n_wb;
}


//----------------- Cache Class functions -------------//
// Cache constructor
// Default constructor
Cache::Cache() {}

Cache::Cache(
	uint32_t block_size, uint32_t size, uint32_t assoc, bool en_prefetch, uint32_t pref_n, uint32_t pref_m
) : block_size(block_size), size(size), assoc(assoc), en_prefetch(en_prefetch), pref_n(pref_n), pref_m(pref_m) {
	
	// DEBUG BEGIN
	//printf("Cache construction\n");
	//printf("blocksize %d, size %d, assoc %d, pref_n %d, pref_m %d\n", block_size, size, assoc, pref_n, pref_m);
	// DEBUG END
	// derive address division
	block_offset_bits = bit_count(block_size);
	num_sets = (uint32_t) size/(block_size*assoc);
	index_bits = bit_count(num_sets);
	tag_bits = ADDR_SIZE - block_offset_bits - index_bits;
	tag_array.resize(num_sets);
	lru.resize(num_sets);
	//lru_position.resize(num_sets);

}

// set the fields of the address in the struct
addr_t Cache::setaddr (uint32_t addr) {
	uint32_t block_offset_mask	= (1<<block_offset_bits) - 1;
	uint32_t index_mask			= (1<<(index_bits+block_offset_bits)) - 1;
	//uint32_t tag_mask 			= (1<<tag_bits) - 1;
	uint32_t tag_mask 			= -1; // all 1s

	addr_t addr_split;
	addr_split.block_offset = block_offset_mask & addr; 
	addr_split.index 		= (index_mask & addr)>>block_offset_bits; 
	addr_split.tag 			= (tag_mask & addr)>>(block_offset_bits+index_bits); 

	// DEBUG BEGIN
	//printf("Parsing Addresses\n");
	//printf("bo bits %d, i bits %d, t bits %d\n",block_offset_bits, index_bits, tag_bits);
	//printf("bo mask %x, i mask %x, t mask %x\n",block_offset_mask, index_mask, tag_mask);
	//printf("bo valu %x, i valu %x, t valu %x\n",addr_split.block_offset, addr_split.index, addr_split.tag);
	// DEBUG END

	return addr_split;
}

// set the pointer to the next level of memory heirarchy
// pointer is NULL if pointing to Main memory
void Cache::setnext (Cache* next) {
	next_level = next;
	return;
}

// pass pointer to main memory access metrics if next heirarchy is mem
void Cache::setmmm (Metrics* main_mem_metrics) {
	mem_metrics = main_mem_metrics;
	return;
}

// Getters
// get iterator to beginning of list (MRU)
list<uint32_t>::iterator Cache::get_lrubegin (int index) {
	return lru[index].begin();
}

// get iterator to end of list (LRU)
list<uint32_t>::iterator Cache::get_lruend (int index) {
	return lru[index].end();
}

// get dirty bit of block
bool Cache::get_dirty (int index, uint32_t tag) {
	return tag_array[index][tag];
}

// get number of sets in cache
uint32_t Cache::get_sets () {
	return num_sets;
}

// Parses the address and reads or writes into cache as per operation
void Cache::request (char rw, uint32_t addr) {

	addr_t addr_split = setaddr(addr);
	cache_metrics.incr_acc();
	
	bool hit = tag_array[addr_split.index].find(addr_split.tag) != tag_array[addr_split.index].end();
	bool evict = tag_array[addr_split.index].size() == assoc;

	//DEBUG BEGIN
	//printf("        %c %x (tag=%x index=%d\n", rw,addr,addr_split.tag,addr_split.index);
	//cout << "        before: ";
	//cout << "set ";
	//cout << right << dec << setw(6) << addr_split.index << ":";
	//// For each way iterate across list from MRU to LRU
	//// check tag stored if it is dirty
	//list<uint32_t>::iterator it;
	////for (it = l1->lru[i].begin(); it != l1->lru[i].end(); it++) {
	//for (it = get_lrubegin(addr_split.index); it != get_lruend(addr_split.index); it++) {
	//	uint32_t tag = (*it);
	//	char dirty = ' ';
	//	//if (l1->tag_array[i][tag])
	//	if (get_dirty(addr_split.index,addr_split.tag))
	//		dirty = 'D';
	//	cout << " ";
	//	cout << right << setw(8) << hex << tag << " " << dirty;
	//}
	//cout << "\n";
	//DEBUG END

	// If HIT
	if (hit) {
		// remove hit way from lru (so that it can be set as most recent later)
		//lru[addr_split.index].erase(lru_position[addr_split.index][addr_split.tag]);
		lru[addr_split.index].remove(addr_split.tag);

	// If MISS
	} else {
		// If no space in cache set
		if (evict) {
			// What is the oldest address (least recently used)
			// Evict that block
			uint32_t old_tag = lru[addr_split.index].back();	
			lru[addr_split.index].pop_back();
			//lru_position[addr_split.index].erase(old_tag);

			// if evicted block is dirty - writeback 
			if (tag_array[addr_split.index][old_tag]) {
				// reverse address computation from lru tag and index
				// Block offset is ignored here since no longer relevant
				uint32_t old_addr = (old_tag<<(block_offset_bits+index_bits)) | (addr_split.index<<(block_offset_bits)); 
				// writeback to evicted (old/least recent) address
				// if next_level is NULL then just update main memory metrics
				cache_metrics.incr_wb();
				if (next_level) {
					next_level->request('w', old_addr);
				} else {
					// main mem metric update	
					mem_metrics->incr_acc();
					mem_metrics->incr_wr();
				}
			}

			// Evict
			tag_array[addr_split.index].erase(old_tag);
		}

		// Then read next level for new block
		// if next_level is NULL then just update main memory metrics
		if (next_level) {
			next_level->request('r', addr);
		} else {
			// main mem metric update	
			mem_metrics->incr_acc();
			mem_metrics->incr_rd();
		}
	}

	// Add accessed block to top of recency list
	lru[addr_split.index].push_front(addr_split.tag);
	//lru_position[addr_split.index][addr_split.tag] = lru[addr_split.index].begin();

	// If read then mark clean
	if (rw == 'r') {
		cache_metrics.incr_rd();
		if (!hit) {
			cache_metrics.incr_rdmiss();
			tag_array[addr_split.index][addr_split.tag] = false;
		}
		//read(addr_split.tag, addr_split.index)
	} else 
	// If write the mark dirty
	if (rw == 'w') {
		cache_metrics.incr_wr();
		if (!hit)
			cache_metrics.incr_wrmiss();
		//write(addr_split.tag, addr_split.index)
		tag_array[addr_split.index][addr_split.tag] = true;
	}

	// DEBUG BEGIN
	//cout << "         after: ";
	//cout << "set ";
	//cout << right << dec << setw(6) << addr_split.index << ":";
	//// For each way iterate across list from MRU to LRU
	//// check tag stored if it is dirty
	////list<uint32_t>::iterator it;
	////for (it = l1->lru[i].begin(); it != l1->lru[i].end(); it++) {
	//for (it = get_lrubegin(addr_split.index); it != get_lruend(addr_split.index); it++) {
	//	uint32_t tag = (*it);
	//	char dirty = ' ';
	//	//if (l1->tag_array[i][tag])
	//	if (get_dirty(addr_split.index,addr_split.tag))
	//		dirty = 'D';
	//	cout << " ";
	//	cout << right << setw(8) << hex << tag << " " << dirty;
	//}
	//cout << "\n";
	//DEBUG END

	return;
}

/*
void Cache::read (uint32_t tag, uint32_t index) {
	bool miss;
	bool evict;
	miss = tag_array[index].find(tag) != tag_array[index].end(); 
	//  incr n_rd
	// If for a particular set the tag exists (HIT)
	if (tag_array[index].find(tag) != tag_array[index].end()) {
		miss = 0;
		//  update lru
	} else {
		miss = 1;
		// incr n_rd_miss 
		// if no space
		// evict
		// else rd next and alloc
		// update lru
	}
}

void Cache::write (uint32_t tag, uint32_t index) {
	
}

void Cache::evict () {
	
}
*/


// -------------------------------------------------------
// Count number of bits needed to count upto the number 
// example: 16 would result in 4 bits (0 to 15)
uint32_t bit_count (uint32_t num) {
	uint32_t bits = 0;

	if (num) {
		while (num) {
			bits++;
			num >>= 1;
		}
		bits--;
	}

	return bits;	
}

// Function for printing results
void print_results(Cache* l1, Cache* l2, Metrics* mem_metrics) {
	
	// TODO print stream buffer contents
	// Print L1 cache contents
	printf("===== L1 contents =====\n");	
	// Iterate over each set
	for (uint32_t i=0; i<l1->get_sets(); i++) {
		cout << "set ";
		cout << right << dec << setw(6) << i << ": ";
		// For each way iterate across list from MRU to LRU
		// check tag stored if it is dirty
		list<uint32_t>::iterator it;
		//for (it = l1->lru[i].begin(); it != l1->lru[i].end(); it++) {
		for (it = l1->get_lrubegin(i); it != l1->get_lruend(i); it++) {
			uint32_t tag = (*it);
			char dirty = ' ';
			//if (l1->tag_array[i][tag])
			if (l1->get_dirty(i,tag))
				dirty = 'D';
			//cout << " ";
			cout << right << setw(8) << hex << tag << " " << dirty;
		}
		cout << "\n";
	}
		
	cout << "\n";
	
	// if l2 exists
	if (l2 != NULL) {
		// Print L2 cache contents
		printf("===== L2 contents =====\n");	
		// Iterate over each set
		for (uint32_t i=0; i<l2->get_sets(); i++) {
			cout << "set ";
			cout << right << dec << setw(6) << i << ": ";
			// For each way iterate across list from MRU to LRU
			// check tag stored if it is dirty
			list<uint32_t>::iterator it;
			//for (it = l2->lru[i].begin(); it != l2->lru[i].end(); it++) {
			for (it = l2->get_lrubegin(i); it != l2->get_lruend(i); it++) {
				uint32_t tag = (*it);
				char dirty = ' ';
				//if (l2->tag_array[i][tag])
				if (l2->get_dirty(i,tag))
					dirty = 'D';
				//cout << " ";
				cout << right << setw(8) << hex << tag << " " << dirty;
			}
			cout << "\n";
		}
		cout << "\n";
	}

	// Print Measurements/Metrics
	uint32_t 	l1_n_acc	= l1->cache_metrics.get_acc();
	uint32_t 	l1_n_rd 	= l1->cache_metrics.get_rd();
	uint32_t 	l1_n_rdmiss	= l1->cache_metrics.get_rdmiss();
	uint32_t 	l1_n_wr 	= l1->cache_metrics.get_wr();
	uint32_t 	l1_n_wrmiss	= l1->cache_metrics.get_wrmiss();
	uint32_t 	l1_n_wb 	= l1->cache_metrics.get_wb();
	float 		l1_mr 		= (float)(l1_n_rdmiss + l1_n_wrmiss)/(l1_n_acc);
	uint32_t 	l1_pf		= 0;
	uint32_t 	l2_n_acc	= 0;
	uint32_t 	l2_n_rd 	= 0;
	uint32_t 	l2_n_rdmiss	= 0;
	uint32_t 	l2_n_wr 	= 0;
	uint32_t 	l2_n_wrmiss	= 0;
	uint32_t 	l2_n_wb 	= 0;
	float 		l2_mr 		= 0;
	uint32_t 	l2_pf		= 0;
	if (l2 != NULL) {
		l2_n_acc	= l2->cache_metrics.get_acc();
		l2_n_rd 	= l2->cache_metrics.get_rd();
		l2_n_rdmiss	= l2->cache_metrics.get_rdmiss();
		l2_n_wr 	= l2->cache_metrics.get_wr();
		l2_n_wrmiss	= l2->cache_metrics.get_wrmiss();
		l2_n_wb 	= l2->cache_metrics.get_wb();
		l2_mr 		= (float)(l2_n_rdmiss)/(l2_n_rd);
		l2_pf		= 0;
	}
	uint32_t mem_n_acc		= mem_metrics->get_acc();

	printf("===== Measurements =====\n");
	printf("a. L1 reads:                   %d\n", l1_n_rd);
	printf("b. L1 read misses:             %d\n", l1_n_rdmiss);
	printf("c. L1 writes:                  %d\n", l1_n_wr);
	printf("d. L1 write misses:            %d\n", l1_n_wrmiss);
	printf("e. L1 miss rate:               %.4f\n", l1_mr);
	printf("f. L1 writebacks:              %d\n", l1_n_wb);
	printf("g. L1 prefetches:              %d\n", l1_pf);
	printf("h. L2 reads (demand):          %d\n", l2_n_rd);
	printf("i. L2 read misses (demand):    %d\n", l2_n_rdmiss);
	printf("j. L2 reads (prefetch):        %d\n", 0);
	printf("k. L2 read misses (prefetch):  %d\n", 0);
	printf("l. L2 writes:                  %d\n", l2_n_wr);
	printf("m. L2 write misses:            %d\n", l2_n_wrmiss);
	printf("n. L2 miss rate:               %.4f\n", l2_mr);
	printf("o. L2 writebacks:              %d\n", l2_n_wb);
	printf("p. L2 prefetches:              %d\n", l2_pf);
	printf("q. memory traffic:             %d\n", mem_n_acc);
		
	return;
}

