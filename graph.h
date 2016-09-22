//graph.h													Wendy Chen, 09/2016

//Contains #include, using, and functions for the graph class

#include <utility>
#include <map>
#include <set>
#include <cstdint> //has type 'uint64_t'

using std::pair;
using std::map;
using std::set;

#ifndef GRAPH_H
#define GRAPH_H

class Graph
{
	private:
		map<uint64_t, set<uint64_t> > nodes;
	public:
		uint64_t add_node(uint64_t node_id);
		uint64_t add_edge(uint64_t node_a_id, uint64_t node_b_id);
		uint64_t remove_node(uint64_t node_id);
		uint64_t remove_edge(uint64_t node_a_id, uint64_t node_b_id);
		uint64_t get_node(uint64_t node_id);
		uint64_t get_edge(uint64_t node_a_id, uint64_t node_b_id);
		pair<uint64_t, set<uint64_t> > get_neighbors(uint64_t node_id);
		pair<uint64_t, uint64_t> shortest_path(uint64_t node_a_id, uint64_t node_b_id);
		void print_set(set<uint64_t> neighbors);
		uint64_t Dijkstra_get_min_distance_unvisited_node(set<uint64_t> unvisited, map<uint64_t, uint64_t> distance);
		set<uint64_t> Dijkstra_get_unvisited_neighbors(set<uint64_t> unvisited, uint64_t node);
};

#endif