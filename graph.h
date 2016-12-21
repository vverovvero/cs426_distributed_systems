//graph.h													Wendy Chen, 09/2016

//Contains #include, using, and functions for the graph class

// #include <utility>
// #include <map>
// #include <set>

// #include "stdint.h"

// using std::pair;
// using std::map;
// using std::set;

#ifndef GRAPH_H
#define GRAPH_H

#include "stdint.h"

#include <iostream>
#include <stdlib.h> //has exit, EXIT_FAILURE
// #include <cstdint> //has type 'uint64_t'
#include <utility> //has type 'pair'
#include <map>
#include <set>
#include <algorithm>

#include <mutex>  

using std::pair; 
using std::map;
using std::set;

const set<uint64_t> EmptySet;
const uint64_t Infinity = UINT64_MAX;

class Graph
{
	private:
		map<uint64_t, set<uint64_t> > nodes;
		std::mutex graph_mtx; /*Mutex for graph lock*/
		bool is_locked = false; /* locked state of graph_mtx, init unlocked */
		unsigned int partition_no = 0;
		unsigned int partition_total = 0;
	public:
		void init_partition_info(unsigned int partition_no, unsigned int partition_info);

		int add_node(uint64_t node_id);
		int add_edge(uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a);
		int remove_edge(uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a);
		int get_node(uint64_t node_id);
		int get_edge(uint64_t node_a_id, uint64_t node_b_id);
		pair<int, set<uint64_t> > get_neighbors(uint64_t node_id);

		void print_set(set<uint64_t> neighbors);
};


#endif