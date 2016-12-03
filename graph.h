//graph.h													Wendy Chen, 09/2016

//Contains #include, using, and functions for the graph class

#include <utility>
#include <map>
#include <set>

#include "stdint.h"

using std::pair;
using std::map;
using std::set;

#ifndef GRAPH_H
#define GRAPH_H

const set<uint64_t> EmptySet;

class Graph
{
	private:
		map<uint64_t, set<uint64_t> > nodes;
	public:
		int add_node(uint64_t node_id){
			// std::cout << "Client would like to add node: " << node_id << std::endl;
			if(get_node(node_id) == 0){
				//Node was not found, create it
				// std::cout << "Adding node with id: " << node_id << std::endl;
				this->nodes.insert(pair<uint64_t, set<uint64_t> >(node_id, EmptySet));
				return 1;
			}
			//Node already exists
			// std::cout << "Cannot add node that already exists" << std::endl;
			return 0;

		}
		int add_edge(uint64_t node_a_id, uint64_t node_b_id);
		int remove_node(uint64_t node_id);
		int remove_edge(uint64_t node_a_id, uint64_t node_b_id);
		int get_node(uint64_t node_id);
		int get_edge(uint64_t node_a_id, uint64_t node_b_id);
		pair<int, set<uint64_t> > get_neighbors(uint64_t node_id);
		pair<int, uint64_t> shortest_path(uint64_t node_a_id, uint64_t node_b_id);
		void print_set(set<uint64_t> neighbors);
		uint64_t Dijkstra_get_min_distance_unvisited_node(set<uint64_t> unvisited, map<uint64_t, uint64_t> distance);
		set<uint64_t> Dijkstra_get_unvisited_neighbors(set<uint64_t> unvisited, uint64_t node);
};

#endif