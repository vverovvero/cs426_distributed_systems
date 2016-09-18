//graph.h													Wendy Chen, 09/2016

//Contains #include, using, and functions for the graph class

#include <utility>
#include <vector>
#include <map>

using std::pair;
using std::vector;
using std::map;

#ifndef GRAPH_H
#define GRAPH_H

class Graph
{
	private:
		map<uint64_t, vector<uint64_t> > nodes;
	public:
		int add_node(uint64_t node_id);
		int add_edge(uint64_t node_a_id, uint64_t node_b_id);
		int remove_node(uint64_t node_id);
		int remove_edge(uint64_t node_a_id, uint64_t node_b_id);
		int get_node(uint64_t node_id);
		int get_edge(uint64_t node_a_id, uint64_t node_b_id);
		pair<int, vector<uint64_t> > get_neighbors(uint64_t node_id);
		pair<int, uint64_t> shortest_path(uint64_t node_a_id, uint64_t node_b_id);
};

#endif