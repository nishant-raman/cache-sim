#ifndef SIM_CACHE_H
#define SIM_CACHE_H

#include <iostream>
#include <tr1/unordered_map>
#include <list>
#include <vector>
#include <iomanip>

#define ADDR_SIZE 32

typedef 
struct {
   uint32_t BLOCKSIZE;
   uint32_t L1_SIZE;
   uint32_t L1_ASSOC;
   uint32_t L2_SIZE;
   uint32_t L2_ASSOC;
   uint32_t PREF_N;
   uint32_t PREF_M;
} cache_params_t;

// Put additional data structures here as per your requirement.

typedef struct {
	uint32_t tag;
	uint32_t index;
	uint32_t block_offset;
} addr_t;

// To track metrics of the cache/mem
class Metrics {
	private:
		uint32_t n_acc;
		uint32_t n_rd;
		uint32_t n_rd_miss;
		uint32_t n_wr;
		uint32_t n_wr_miss;
		uint32_t n_wb;
		uint32_t n_pf; // no. of pf to next level
		uint32_t n_rd_pf; // no. of rd due to pf from prev level
		uint32_t n_rd_miss_pf; // no. of rdmiss due to pf from prev level

	public:
		//Metrics(uint32_t n_acc, uint32_t n_rd, uint32_t n_rd_miss, uint32_t n_wr, uint32_t n_wr_miss, uint32_t n_wb);
		Metrics();
		
		void 		incr_acc ();
		void 		incr_rd ();
		void 		incr_rdmiss ();
		void 		incr_wr ();
		void 		incr_wrmiss ();
		void 		incr_wb ();
		void 		incr_pf ();
		void 		incr_rd_pf ();
		void 		incr_rdmiss_pf ();
		uint32_t 	get_acc ();
		uint32_t 	get_rd ();
		uint32_t 	get_rdmiss ();
		uint32_t 	get_wr ();
		uint32_t 	get_wrmiss ();
		uint32_t 	get_wb ();
		uint32_t 	get_pf ();
		uint32_t 	get_rd_pf ();
		uint32_t 	get_rdmiss_pf ();

};

// Cache Class
// Only write-back write-allocate
class Cache {
	private:
		// params
		uint32_t 	block_size;
		uint32_t 	size;
		uint32_t 	assoc;
		bool 		en_prefetch;
		uint32_t 	pref_n;
		uint32_t 	pref_m;

		// Address fields
		uint32_t 	tag_bits;
		uint32_t 	index_bits;
		uint32_t 	block_offset_bits;
		uint32_t 	num_sets;

		// Next Memory Heirarchy
		Cache* next_level;

		// Tag array
		// tag array is an array of sets 
		// each set is an unordered map whose key is the tag and value is the dirty bit
		// valid is implicitly understood by the existence of an element
		// the LRU algorithm is realised as a list/queue instead of counters
		std::vector<std::tr1::unordered_map<uint32_t, bool> > tag_array;
		std::vector<std::list<uint32_t> > cache_lru;
		//std::vector<std::tr1::unordered_map<uint32_t, std::list<uint32_t>::iterator> > lru_position;

		addr_t setaddr (uint32_t addr);

		// Stream Buffer
		// smallest element in strmbuff contains the block address
		// TODO getter setter constr
		std::list<uint32_t> strmbuff_lru;
		std::vector<std::list<uint32_t> > strmbuff;
		
	public:
		Metrics cache_metrics;	
		Metrics* mem_metrics;
	
		Cache();
		Cache(uint32_t block_size, uint32_t size, uint32_t assoc, bool en_prefetch, uint32_t pref_n, uint32_t pref_m);

		// Public function to access cache for a particular address
		void request (char rw, uint32_t addr, bool pf);
		void evict (uint32_t tag, uint32_t index);
		void updlru (std::list<uint32_t> & lru, bool hit, bool evict, uint32_t tag);
		void setnext (Cache* next);
		void setmmm (Metrics* main_mem_metrics);
		std::list<uint32_t>::iterator get_strmbuffbegin (uint32_t index);
		std::list<uint32_t>::iterator get_strmbuffend (uint32_t index);
		std::list<uint32_t>::iterator get_lrubegin (uint32_t index);
		std::list<uint32_t>::iterator get_lruend (uint32_t index);
		std::list<uint32_t>::iterator get_lrubegin ();
		std::list<uint32_t>::iterator get_lruend ();
		bool get_dirty (uint32_t index, uint32_t tag);
		uint32_t get_sets ();
		bool get_pfen ();
		uint32_t get_pfm ();
		uint32_t get_pfn ();
		bool searchstrmbuff (uint32_t block_addr, uint32_t& strmbuff_num, uint32_t& pf_degree); // update strmbuff num and degree to be used in prefetch fxn
		void prefetch (uint32_t addr, uint32_t strmbuff_num, uint32_t pf_degree);

};

uint32_t bit_count (uint32_t num);
void print_results(Cache* l1, Cache* l2, Metrics* mem_metrics);

#endif
