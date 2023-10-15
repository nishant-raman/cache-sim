#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "sim.h"
#include <algorithm>

using namespace std;

#define DBG 0

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

	if (!DBG) {
	printf("\n");
	}
	
	// DEBUG TRACE BEGIN
	int dbgidx = 0;
	// DEBUG TRACE END
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
		// DEBUG TRACE BEGIN
		if (DBG) {
			dbgidx++;
			if (rw == 'r') {
				printf("%d=r %x\n", dbgidx,addr);
			} else {
				printf("%d=w %x\n", dbgidx,addr);
			}
		}
		// DEBUG TRACE END

	///////////////////////////////////////////////////////
	// Issue the request to the L1 cache instance here.
	///////////////////////////////////////////////////////
		l1->request(rw, addr, 0);

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

// Increment number of rd of current cache due to prefetches
void Metrics::incr_rd_pf () {
	n_rd_pf++;
	return;
}

// Increment number of read misses of current cache due to prefetches
void Metrics::incr_rdmiss_pf () {
	n_rd_miss_pf++;
	return;
}

// Increment number of prefetches of current cache to next level
void Metrics::incr_pf () {
	//DEBUG BEGIN
	//printf("Number of prefetches so far: %d\n", n_pf);
	//DEBUG END
	n_pf++;
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

uint32_t Metrics::get_rd_pf () {
	return n_rd_pf;
}

uint32_t Metrics::get_rdmiss_pf () {
	return n_rd_miss_pf;
}

uint32_t Metrics::get_pf () {
	return n_pf;
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
	cache_lru.resize(num_sets);
	//lru_position.resize(num_sets);
	if (en_prefetch) {
		strmbuff.resize(pref_n);
		for (uint32_t i=0; i<pref_n; i++) {
			strmbuff_lru.push_back(i);	
		}
	}

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
list<uint32_t>::iterator Cache::get_lrubegin () {
	return strmbuff_lru.begin();
}

// get iterator to end of list (LRU)
list<uint32_t>::iterator Cache::get_lruend () {
	return strmbuff_lru.end();
}

// get iterator to beginning of list (MRU)
list<uint32_t>::iterator Cache::get_lrubegin (uint32_t index) {
	return cache_lru[index].begin();
}

// get iterator to end of list (LRU)
list<uint32_t>::iterator Cache::get_lruend (uint32_t index) {
	return cache_lru[index].end();
}

// get iterator to beginning of list (MRU)
list<uint32_t>::iterator Cache::get_strmbuffbegin (uint32_t index) {
	return strmbuff[index].begin();
}

// get iterator to end of list (LRU)
list<uint32_t>::iterator Cache::get_strmbuffend (uint32_t index) {
	return strmbuff[index].end();
}

// get dirty bit of block
bool Cache::get_dirty (uint32_t index, uint32_t tag) {
	return tag_array[index][tag];
}

// get number of sets in cache
uint32_t Cache::get_sets () {
	return num_sets;
}

// get if prefetch is enabled
bool Cache::get_pfen () {
	return en_prefetch;
}

// get pref n value
uint32_t Cache::get_pfn () {
	return pref_n;
}

// get pref n value
uint32_t Cache::get_pfm () {
	return pref_m;
}

 /*
// Parses the address and reads or writes into cache as per operation
void Cache::request (char rw, uint32_t addr, bool pf) {

	// FIXME if pf set update lru normally?

	addr_t addr_split = setaddr(addr);
	cache_metrics.incr_acc();
	
	bool hit = tag_array[addr_split.index].find(addr_split.tag) != tag_array[addr_split.index].end();
	bool eviction = tag_array[addr_split.index].size() == assoc;

	//DEBUG BEGIN
	//printf("        %c %x (tag=%x index=%d\n", rw,addr,addr_split.tag,addr_split.index);
	//cout << "        before: ";
	//cout << "set ";
	//cout << right << dec << setw(6) << addr_split.index << ":";
	//// For each way iterate across list from MRU to LRU
	//// check tag stored if it is dirty
	//list<uint32_t>::iterator it;
	////for (it = l1->cache_lru[i].begin(); it != l1->cache_lru[i].end(); it++) {
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
		//cache_lru[addr_split.index].erase(lru_position[addr_split.index][addr_split.tag]);
		cache_lru[addr_split.index].remove(addr_split.tag);

	// If MISS
	} else {
		// If no space in cache set
		if (eviction) {
			// What is the oldest address (least recently used)
			// Evict that block
			uint32_t old_tag = cache_lru[addr_split.index].back();	
			cache_lru[addr_split.index].pop_back();
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
					next_level->request('w', old_addr, 0);
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
			next_level->request('r', addr, 0);
		} else {
			// main mem metric update	
			mem_metrics->incr_acc();
			mem_metrics->incr_rd();
		}
	}

	// Add accessed block to top of recency list
	cache_lru[addr_split.index].push_front(addr_split.tag);
	//lru_position[addr_split.index][addr_split.tag] = cache_lru[addr_split.index].begin();

	// If read then mark clean
	// If request is sourced from prefetch then incr pf related metrics
	if (rw == 'r') {
		if (pf) {
			cache_metrics.incr_rd_pf();
		} else {
			cache_metrics.incr_rd();
		}
			
		if (!hit) {
			if (pf) {
				cache_metrics.incr_rdmiss_pf();
			} else {
				cache_metrics.incr_rdmiss();
			}
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
	////for (it = l1->cache_lru[i].begin(); it != l1->cache_lru[i].end(); it++) {
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
*/


// General Cache request function handling prefetches as well
void Cache::request (char rw, uint32_t addr, bool pf) {
	
	cache_metrics.incr_acc();
	addr_t addr_split 		= setaddr(addr);
	//uint32_t block_address 	= addr & ~((1<<block_offset_bits)-1);
	uint32_t block_addr 	= addr >> (block_offset_bits);

	bool cache_hit = tag_array[addr_split.index].find(addr_split.tag) != tag_array[addr_split.index].end();
	bool eviction = tag_array[addr_split.index].size() == assoc;
	bool prefetch_hit = false; // Default set to 0 as prefetch disabled

	uint32_t strmbuff_num = strmbuff_lru.back(); 	// index of stream buffer (n) - by default oldest
	uint32_t pf_degree = pref_m;	// how many blocks to prefetch OR at which block did prefetcher hit
	//DEBUG BEGIN
	//printf("Just init, pf deg: %d\n",pf_degree);
	//printf("Just init, sb num: %d\n",strmbuff_num);
	//DEBUGEND
	
	//DEBUG TRACE BEGIN
	/*
	if (DBG) {
		printf("        %c %x (tag=%x index=%d\n", rw,addr,addr_split.tag,addr_split.index);
		cout << "        before: ";
		cout << "set ";
		cout << right << dec << setw(6) << addr_split.index << ":";
		// For each way iterate across list from MRU to LRU
		// check tag stored if it is dirty
		list<uint32_t>::iterator it;
		for (it = get_lrubegin(addr_split.index); it != get_lruend(addr_split.index); it++) {
			uint32_t tag = (*it);
			char dirty = ' ';
			if (get_dirty(addr_split.index,tag))
				dirty = 'D';
			cout << " ";
			cout << right << setw(8) << hex << tag << " " << dirty;
		}
		cout << "\n";
	}
	*/
	//DEBUG TRACE END


	if (en_prefetch) {
		prefetch_hit = searchstrmbuff(block_addr, strmbuff_num, pf_degree);
		//prefetch_hit = searchstrmbuff(addr, strmbuff_num, pf_degree);
		//DEBUG BEGIN
		//printf("pf en, pf hit? %d, pf deg: %d\n",prefetch_hit,pf_degree);
		//printf("pf en pf hit?, sb num: %d\n",strmbuff_num);
		//DEBUGEND
		if (!prefetch_hit) {
			strmbuff_num = strmbuff_lru.back();
			pf_degree = pref_m;
			//DEBUG BEGIN
			//printf("pf en, no pf hit? %d, pf deg upd: %d\n",prefetch_hit,pf_degree);
			//printf("pf en no pf hit?, sb num: %d\n",strmbuff_num);
			//DEBUGEND
		}
	}
	//DEBUG BEGIN
	//printf("Cache hit? %d, Pref en? %d, Prefetch hit? %d\n", cache_hit, en_prefetch, prefetch_hit);
	// DEBUG END

	// If Missed in Tag Array
	if (!cache_hit) {
		// If no space in set	
		if (eviction) {
			evict(addr_split.tag, addr_split.index);
		}

		// Request and allocate block
		// If its a miss and the prefetcher also misses
		// Cache requests from next level (whether prefetcher exists or not)
		if (!prefetch_hit) {
			// Read next level for new block
			// if next_level is NULL then just update main memory metrics
			if (next_level) {
				next_level->request('r', addr, 0);
			} else {
				// main mem metric update	
				mem_metrics->incr_acc();
				mem_metrics->incr_rd();
			}

			// If prefetcher is enabled initiate prefetch of full buffer size
			if (en_prefetch) {
			// DEBUG BEGIN
			//printf("size of buff %d\n",strmbuff[strmbuff_num].size());
			//printf("pref deg %d\n",pf_degree);
			// DEBUG END
				// prefetch(M,old)
				prefetch(addr, strmbuff_num, pf_degree);
			}

		// if prefetch hits then update prefetch to keep it timely
		} else {
			// prefetch(k,n)
			prefetch(addr, strmbuff_num, pf_degree);
		}

	// If HIT
	} else {
		// If prefetcher also hits then update and keep timely
		if (prefetch_hit) {
			// prefetch(k,n)
			prefetch(addr, strmbuff_num, pf_degree);
		}
		//
	}

	// Stream Buffer LRU Update
	if (en_prefetch && !(cache_hit && !prefetch_hit)) {
		updlru(strmbuff_lru, prefetch_hit, !prefetch_hit, strmbuff_num);
	}
	// Cache LRU Update
	updlru(cache_lru[addr_split.index], cache_hit, eviction, addr_split.tag);
	
	// Allocate Blocks
	// If read miss then mark clean
	// If request is sourced from prefetch then incr pf related metrics
	if (rw == 'r') {
		if (pf) {
			cache_metrics.incr_rd_pf();
		} else {
			cache_metrics.incr_rd();
		}
			
		if (!cache_hit && !prefetch_hit) {
			if (pf) {
				cache_metrics.incr_rdmiss_pf();
			} else {
				cache_metrics.incr_rdmiss();
			}
			tag_array[addr_split.index][addr_split.tag] = false;
		}
		//read(addr_split.tag, addr_split.index)
	} else 
	// If write the mark dirty
	if (rw == 'w') {
		cache_metrics.incr_wr();
		if (!cache_hit && !prefetch_hit) {
			cache_metrics.incr_wrmiss();
		}
		//write(addr_split.tag, addr_split.index)
		tag_array[addr_split.index][addr_split.tag] = true;
	}
	
	// WHAT IS HAPPENING??!!
	// DEBUG TRACE BEGIN
	//if (DBG) {
		//cout << "         after: ";
		//cout << "set ";
		//cout << right << dec << setw(6) << addr_split.index << ":";
		// For each way iterate across list from MRU to LRU
		// check tag stored if it is dirty
		list<uint32_t>::iterator it;
		for (it = get_lrubegin(addr_split.index); it != get_lruend(addr_split.index); it++) {
			uint32_t tag = (*it);
			char dirty = ' ';
			//if (l1->tag_array[i][tag])
			if (get_dirty(addr_split.index,tag))
				dirty = 'D';
			//cout << " ";
			//cout << right << setw(8) << hex << tag << " " << dirty;
		}
		//cout << "\n";
	//}
	// DEBUG TRACE END
	// DEBUG TRACE BEGIN
	/*
	if (DBG) {
		cout << "		PrefetchDegree: " << pf_degree << "	Stream Buffer Number: " << strmbuff_num << "\n";
		cout << "		";
		if(cache_hit) {
			cout << " CacheHit ";
		} else {
			cout << "CacheMiss "; 
		}
		if (prefetch_hit) {
			cout << " PrefetchHit\n";
		} else {
			cout << "PrefetchMiss\n";
		}
		if (en_prefetch && !(cache_hit && !prefetch_hit)) {
			list<uint32_t>::iterator it1;
			list<uint32_t>::iterator it2;
			for (it1 = get_lrubegin(); it1 != get_lruend(); it1++) {
				if (get_strmbuffbegin(*it1) != get_strmbuffend(*it1)) {
					cout <<  "			SB: ";
					for (it2 = get_strmbuffbegin(*it1); it2 != get_strmbuffend(*it1); it2++) {
						cout << ' ';
						cout << right << setw(7) << hex << (*it2);
						cout << ' ';
					}
					cout << '\n';
				}
			}
		}
	}
	*/
	//// DEBUG TRACE END


	return;

}

// Cache block allocation function (miss or hit)
//void tagalloc (uint32_t tag, uint32_t index, bool c_hit, bool pf_hit, bool evict) {
// Cache block eviction
void Cache::evict (uint32_t tag, uint32_t index) {

		// What is the oldest address (least recently used)
		uint32_t old_tag = cache_lru[index].back();	
		
		// if evicted block is dirty - writeback 
		if (tag_array[index][old_tag]) {
			// reverse address computation from lru tag and index
			// Block offset is ignored here since no longer relevant
			uint32_t old_addr = (old_tag<<(block_offset_bits+index_bits)) | (index<<(block_offset_bits)); 
			// writeback to evicted (old/least recent) address
			// if next_level is NULL then just update main memory metrics
			cache_metrics.incr_wb();
			if (next_level) {
				next_level->request('w', old_addr, 0);
			} else {
				// main mem metric update	
				mem_metrics->incr_acc();
				mem_metrics->incr_wr();
			}
		}

		// Evict
		tag_array[index].erase(old_tag);

	return;

}

// General function for managing an LRU algorithm list/queue
// NOTE:
// In the context of the cache lru the tag is used to identify the tag array
// 		which exists as an unordered map
// In the context of the stream buffer the tag is used to identify which buffer is MRU/LRU
// 		so it is the strmbuff_num (even though referred to as tag here)
// 		thus it requires explicitly finding and passing the old "tag" in case of evictions
void Cache::updlru (list<uint32_t> & lru, bool hit, bool eviction, uint32_t tag) {
	
	// If HIT
	if (hit) {
		// remove hit way from lru (so that it can be set as most recent later)
		lru.remove(tag);
	// If MISS
	} else {
		// If no space in cache set
		if (eviction) {
			// Update LRU to show eviction of oldest block
			lru.pop_back();
		}
	}

	// Add accessed block to top of recency list
	lru.push_front(tag);
}

// Search prefetch buffer for referenced block
bool Cache::searchstrmbuff (uint32_t block_addr, uint32_t& strmbuff_num, uint32_t& pf_degree) {

	// mask block offset bits in the address	
	//addr &= ~((1<<block_offset_bits)-1);

	// DEBUG BEGIN
	//printf("searching sb, addr: %x\n", addr);
	// DEBUG END
	// Search from MRU to LRU stream buffer
	list<uint32_t>::iterator itlru;
	for (itlru=strmbuff_lru.begin(); itlru!=strmbuff_lru.end(); itlru++) {
		strmbuff_num = *itlru;
		// DEBUG BEGIN
		//printf("comparing1, addr: %d\n", *itlru);
		//printf("comparing1, addr1: %d\n", strmbuff_num);
		// DEBUG END
		
		// Search a buffer for the block address
		list<uint32_t>::iterator itbuf;
		pf_degree = 0;
		for (itbuf=strmbuff[strmbuff_num].begin(); itbuf!=strmbuff[strmbuff_num].end(); itbuf++) {
			pf_degree++;
				// DEBUG BEGIN
				//printf("comparing, addr: %x\n", *itbuf);
				// DEBUG END
			//if ((*itbuf) == (addr)) {
			if ((*itbuf) == (block_addr)) {
			//if ((*itbuf) == (addr>>block_offset_bits)) {
			//if (((*itbuf)>>block_offset_bits) == (addr>>block_offset_bits)) {
				return true;
			}
		}
	}

	return false;
} 

// Function to prefetch blocks from next heirarchy
// it will remove elements at and ahead of hit location, request next level for as many blocks, and alloc
// If it is a miss in the stream buffer it should get values indicating last element of lru buffer
void Cache::prefetch (uint32_t addr, uint32_t strmbuff_num, uint32_t pf_degree) {
	
	// TODO FIXME number of prefetches is 1 for stream or n?
	// TODO FIXME similarly does a prefetch generate 1 access or n?
	cache_metrics.incr_pf();

	// DEBUG BEGIN
	//printf("Prefetching\n");
	//printf("At top : %x\n", strmbuff[strmbuff_num].front());
	//printf("Prefetch Degeww : %d\n", pf_degree);
	//printf("Stream buffer number : %d\n", strmbuff_num);
	// DEBUG END

	int buffer_size = strmbuff[strmbuff_num].size();

	for (uint32_t i=0; i<pf_degree; i++) {
		//cache_metrics.incr_pf();
		//DEBUG BEGIN
		//printf("contents front %x\n",strmbuff[strmbuff_num].front());
		//DEBUG END
		if (buffer_size) {
			strmbuff[strmbuff_num].pop_front();
		}
		if (next_level) {
			next_level->request('r',addr,true);
		} else {
			// main mem metric update	
			mem_metrics->incr_acc();
			mem_metrics->incr_rd();
		}
		strmbuff[strmbuff_num].push_back((addr>>block_offset_bits)+1+i);
		//DEBUG BEGIN
		//printf("contents back %x\n",strmbuff[strmbuff_num].back());
		//DEBUG END
	}
	

	return;
}

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
		for (it = l1->get_lrubegin(i); it != l1->get_lruend(i); it++) {
			uint32_t tag = (*it);
			char dirty = ' ';
			if (l1->get_dirty(i,tag))
				dirty = 'D';
			//cout << " ";
			cout << right << setw(8) << hex << tag << " " << dirty;
		}
		cout << "\n";
	}
		
	cout << "\n";
	// TODO FIXME dont print set if no valid blocks
	// TODO FIXME dont print invalid sets
	
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
			//for (it = l2->cache_lru[i].begin(); it != l2->cache_lru[i].end(); it++) {
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

	if (l1->get_pfen()) {
		// TODO FIXME do not print invalid contents
		// Print L1 Stream Buffer Contents
		printf("===== Stream Buffer(s) contents =====\n");
		// Iterate over each buffer from MRU to LRU
		list<uint32_t>::iterator it1;
		list<uint32_t>::iterator it2;
		for (it1 = l1->get_lrubegin(); it1 != l1->get_lruend(); it1++) {
			for (it2 = l1->get_strmbuffbegin(*it1); it2 != l1->get_strmbuffend(*it1); it2++) {
				cout << ' ';
				cout << (*it2);
				cout << ' ';
			}
			cout << '\n';
		}
		cout << '\n';
	}

	// Print Measurements/Metrics
	uint32_t 	l1_n_acc	= l1->cache_metrics.get_acc();
	uint32_t 	l1_n_rd 	= l1->cache_metrics.get_rd();
	uint32_t 	l1_n_rdmiss	= l1->cache_metrics.get_rdmiss();
	uint32_t 	l1_n_wr 	= l1->cache_metrics.get_wr();
	uint32_t 	l1_n_wrmiss	= l1->cache_metrics.get_wrmiss();
	uint32_t 	l1_n_wb 	= l1->cache_metrics.get_wb();
	float 		l1_mr 		= (float)(l1_n_rdmiss + l1_n_wrmiss)/(l1_n_acc);
	uint32_t 	l1_n_pf		= l1->cache_metrics.get_pf();
	uint32_t 	l2_n_acc	= 0;
	uint32_t 	l2_n_rd 	= 0;
	uint32_t 	l2_n_rdmiss	= 0;
	uint32_t 	l2_n_wr 	= 0;
	uint32_t 	l2_n_wrmiss	= 0;
	uint32_t 	l2_n_wb 	= 0;
	float 		l2_mr 		= 0;
	uint32_t 	l2_n_pf		= 0;
	uint32_t 	l2_n_rd_pf 	= 0;
	uint32_t 	l2_n_rdmiss_pf	= 0;
	if (l2 != NULL) {
		l2_n_acc	= l2->cache_metrics.get_acc();
		l2_n_rd 	= l2->cache_metrics.get_rd();
		l2_n_rdmiss	= l2->cache_metrics.get_rdmiss();
		l2_n_wr 	= l2->cache_metrics.get_wr();
		l2_n_wrmiss	= l2->cache_metrics.get_wrmiss();
		l2_n_wb 	= l2->cache_metrics.get_wb();
		l2_mr 		= (float)(l2_n_rdmiss)/(l2_n_rd);
		l2_n_pf		= l2->cache_metrics.get_pf();
		l2_n_rd_pf	= l2->cache_metrics.get_rd_pf();
		l2_n_rdmiss_pf		= l2->cache_metrics.get_rdmiss_pf();
	}
	uint32_t mem_n_acc		= mem_metrics->get_acc();

	printf("===== Measurements =====\n");
	printf("a. L1 reads:                   %d\n", l1_n_rd);
	printf("b. L1 read misses:             %d\n", l1_n_rdmiss);
	printf("c. L1 writes:                  %d\n", l1_n_wr);
	printf("d. L1 write misses:            %d\n", l1_n_wrmiss);
	printf("e. L1 miss rate:               %.4f\n", l1_mr);
	printf("f. L1 writebacks:              %d\n", l1_n_wb);
	printf("g. L1 prefetches:              %d\n", l1_n_pf);
	printf("h. L2 reads (demand):          %d\n", l2_n_rd);
	printf("i. L2 read misses (demand):    %d\n", l2_n_rdmiss);
	printf("j. L2 reads (prefetch):        %d\n", l2_n_rd_pf);
	printf("k. L2 read misses (prefetch):  %d\n", l2_n_rdmiss_pf);
	printf("l. L2 writes:                  %d\n", l2_n_wr);
	printf("m. L2 write misses:            %d\n", l2_n_wrmiss);
	printf("n. L2 miss rate:               %.4f\n", l2_mr);
	printf("o. L2 writebacks:              %d\n", l2_n_wb);
	printf("p. L2 prefetches:              %d\n", l2_n_pf);
	printf("q. memory traffic:             %d\n", mem_n_acc);
		
	return;
}

