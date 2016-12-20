//graph.cpp 										Wendy Chen, 09/2016

//Implements an adjacency list, using a map to store
//(node, neighbor-list) pairs, where each neighbor list
//is vector<node>.  
//Class is defined in graph.h

#include "graph.h"

#include "stdint.h"

#include <iostream>
#include <stdlib.h> //has exit, EXIT_FAILURE
// #include <cstdint> //has type 'uint64_t'
#include <utility> //has type 'pair'
#include <map>
#include <set>
#include <algorithm>


using std::pair; 
using std::map;
using std::set;

// const set<uint64_t> EmptySet;
// const uint64_t Infinity = UINT64_MAX;

//Be consistent on what means success
// 1 is success
// 0 is fail
// 2 is extra option

//////////////////////////////////////////////////////////////////////
//Helper functions

//prints contents of a set
void Graph::print_set(set<uint64_t> neighbors){
	std::cout << "Printing set:" << std::endl;
	for(set<uint64_t>::iterator i = neighbors.begin(); i != neighbors.end(); i++){
		uint64_t neighbor = *i;
		std::cout << neighbor << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////
//return 1 if node is in the graph
//return 0 if node is not in the graph
int Graph::get_node(uint64_t node_id){
	// std::cout << "Getting node: " << node_id << std::endl;
	map<uint64_t, set<uint64_t> >::iterator it;
	it = this->nodes.find(node_id);
	if(it != this->nodes.end()){
		//Found the node
		// std::cout << "Found the node " << it->first << std::endl;
		return 1;
	}
	//Node was not found
	// std::cout << "Node was not found" << std::endl;
	return 0;
}

//return 1 if edge is in graph
//return 0 if edge is not in graph
//return 2 if at least one of vertices does not exist (or if nodes are same?)
int Graph::get_edge(uint64_t node_a_id, uint64_t node_b_id){
	// std::cout << "Getting edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	//are the nodes the same?
	if(node_a_id == node_b_id){
		// std::cout << "Nodes are the same" << std::endl;
		return 2;
	}
	//do the nodes exist?
	map<uint64_t, set<uint64_t> >::iterator it_a, it_b;
	it_a = this->nodes.find(node_a_id);
	it_b = this->nodes.find(node_b_id);
	if(it_a == this->nodes.end() || it_b == this->nodes.end()){
		// std::cout << "One or both nodes do not exist" << std::endl;
		return 2;
	}
	//Nodes exist and have unique ids
	//Does the edge already exist?
	set<uint64_t> &node_a_neighbors = it_a->second;
	set<uint64_t> &node_b_neighbors = it_b->second;
	set<uint64_t>::iterator it_a_neighbors;
	set<uint64_t>::iterator it_b_neighbors;
	it_a_neighbors = node_a_neighbors.find(node_b_id);
	it_b_neighbors = node_b_neighbors.find(node_a_id);
	if(it_a_neighbors != node_a_neighbors.end() && it_b_neighbors != node_b_neighbors.end()){
		//Edge was found.  Don't add it.
		// std::cout << "Edge was found (on both nodes)." << std::endl;
		return 1;
	}
	
	//Edge doesn't exist, add it
	// std::cout << "Edge doesn't exist.  Was not found." << std::endl;
	return 0;
}

//return 1 if success, 0 if node already exists
int Graph::add_node(uint64_t node_id){
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

//return 1 if success, 0 if edge already exists,
//2 if node doesn't exist or node_a_id == node_b_id
int Graph::add_edge(uint64_t node_a_id, uint64_t node_b_id){
	// std::cout << "Client wants to add edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	int found_edge = get_edge(node_a_id, node_b_id);
	if(found_edge == 2){
		// std::cout << "Cannot add edge because node(s) doesn't exist, or is same." << std::endl;
		return 2;
	}
	else if(found_edge == 1){
		// std::cout << "Edge already in graph.  Do not add." << std::endl;
		return 0;
	}
	else{
		// std::cout << "Adding edge!" << std::endl;
		map<uint64_t, set<uint64_t> >::iterator it_a, it_b;
		it_a = this->nodes.find(node_a_id);
		it_b = this->nodes.find(node_b_id);
		set<uint64_t> &node_a_neighbors = it_a->second;
		set<uint64_t> &node_b_neighbors = it_b->second;
		node_a_neighbors.insert(node_b_id);
		node_b_neighbors.insert(node_a_id);
		return 1;
	}
}


//return 1 if success, 0 if edge does not exist
int Graph::remove_edge(uint64_t node_a_id, uint64_t node_b_id){
	// std::cout << "Client wants to remove edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	//does edge exist?  Check neighbors of both nodes
	int found_edge = get_edge(node_a_id, node_b_id);
	if(found_edge == 1){
		// std::cout << "Removing edge!" << std::endl;
		//Edge is in graph.  Get neighbors for each node, and remove the opposite 
		this->nodes[node_a_id].erase(node_b_id);
		this->nodes[node_b_id].erase(node_a_id);

		return 1;
	}
	//No edge to remove.
	// std::cout << "No edge to remove." << std::endl;
	return 0;
}


//return pair (success/fail, vector of neighbors)
//return (1, set of neighbors) if node exists
//return (0, empty set) if node doesn't exist
pair<int, set<uint64_t> > Graph::get_neighbors(uint64_t node_id){
	// std::cout << "Client would like to get neighbors of node: " << node_id << std::endl;
	//first find node
	int found_node = get_node(node_id);
	if(found_node == 1){
		//Node found.  Return the set of neighbors.
		return pair<int, set<uint64_t> > (1, this->nodes[node_id]);
	}
	// std::cout << "Node doesn't exist.  No neighbors to fetch." << std::endl;
	return pair<int, set<uint64_t> > (0, EmptySet); //is this correct?
}

