//graph.cpp 										Wendy Chen, 09/2016

//Implements an adjacency list, using a map to store
//(node, neighbor-list) pairs, where each neighbor list
//is vector<node>.  
//Class is defined in graph.h

#include "graph.h"

#include <iostream>
#include <cstdint> //has type 'uint64_t'
#include <utility> //has type 'pair'
#include <map>
#include <set>

using std::pair; 
using std::map;
using std::set;

const set<uint64_t> EmptySet;
const uint64_t NoDistance = 0;

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
	std::cout << "Getting node: " << node_id << std::endl;
	map<uint64_t, set<uint64_t> >::iterator it;
	it = this->nodes.find(node_id);
	if(it != this->nodes.end()){
		//Found the node
		std::cout << "Found the node " << it->first << std::endl;
		return 1;
	}
	//Node was not found
	std::cout << "Node was not found" << std::endl;
	return 0;
}

//return 1 if edge is in graph
//return 0 if edge is not in graph
//return 2 if at least one of vertices does not exist (or if nodes are same?)
int Graph::get_edge(uint64_t node_a_id, uint64_t node_b_id){
	std::cout << "Getting edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	//are the nodes the same?
	if(node_a_id == node_b_id){
		std::cout << "Nodes are the same" << std::endl;
		return 2;
	}
	//do the nodes exist?
	map<uint64_t, set<uint64_t> >::iterator it_a, it_b;
	it_a = this->nodes.find(node_a_id);
	it_b = this->nodes.find(node_b_id);
	if(it_a == this->nodes.end() || it_b == this->nodes.end()){
		std::cout << "One or both nodes do not exist" << std::endl;
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
		std::cout << "Edge was found (on both nodes)." << std::endl;
		return 1;
	}
	
	//Edge doesn't exist, add it
	std::cout << "Edge doesn't exist.  Was not found." << std::endl;
	return 0;
}

//return 1 if success, 0 if node already exists
int Graph::add_node(uint64_t node_id){
	std::cout << "Client would like to add node: " << node_id << std::endl;
	if(get_node(node_id) == 0){
		//Node was not found, create it
		std::cout << "Adding node with id: " << node_id << std::endl;
		this->nodes.insert(pair<uint64_t, set<uint64_t> >(node_id, EmptySet));
		return 1;
	}
	//Node already exists
	std::cout << "Cannot add node that already exists" << std::endl;
	return 0;

}

//return 1 if success, 0 if edge already exists,
//2 if node doesn't exist or node_a_id == node_b_id
int Graph::add_edge(uint64_t node_a_id, uint64_t node_b_id){
	std::cout << "Client wants to add edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	int found_edge = get_edge(node_a_id, node_b_id);
	if(found_edge == 2){
		std::cout << "Cannot add edge because node(s) doesn't exist, or is same." << std::endl;
		return 2;
	}
	else if(found_edge == 1){
		std::cout << "Edge already in graph.  Do not add." << std::endl;
		return 0;
	}
	else{
		std::cout << "Adding edge!" << std::endl;
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


//return 1 if success, 0 if node does not exist
int Graph::remove_node(uint64_t node_id){
	std::cout << "Client would like to erase node: " << node_id << std::endl;
	//Call erase.  If no items erased, then node did not exist
	if(this->nodes.erase(node_id) != 0){
		//Node(s) was erased.
		std::cout << "Node was erased" << std::endl;
		return 1;
	}
	//No node erased.
	std::cout << "Node to be erased did not exist." << std::endl;
	return 0;
}

//return 1 if success, 0 if edge does not exist
int Graph::remove_edge(uint64_t node_a_id, uint64_t node_b_id){
	std::cout << "Client wants to remove edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	//does edge exist?  Check neighbors of both nodes
	int found_edge = get_edge(node_a_id, node_b_id);
	if(found_edge == 1){
		std::cout << "Removing edge!" << std::endl;
		//Edge is in graph.  Need to remove from both nodes.
		map<uint64_t, set<uint64_t> >::iterator it_a, it_b;
		set<uint64_t> it_a_neighbors = this->nodes.find(node_a_id)->second;
		set<uint64_t> it_b_neighbors = this->nodes.find(node_b_id)->second;

		//Remove from each neighbor list.
		it_a_neighbors.erase(node_b_id);
		it_b_neighbors.erase(node_a_id);
		return 1;
	}
	//No edge to remove.
	std::cout << "No edge to remove." << std::endl;
	return 0;
}


//return pair (success/fail, vector of neighbors)
//return (1, set of neighbors) if node exists
//return (0, empty set) if node doesn't exist
pair<int, set<uint64_t> > Graph::get_neighbors(uint64_t node_id){
	std::cout << "Client would like to get neighbors of node: " << node_id << std::endl;
	//first find node
	int found_node = get_node(node_id);
	if(found_node == 1){
		//Node found.  Return the set of neighbors.
		map<uint64_t, set<uint64_t> >::iterator it;
		it = this->nodes.find(node_id);
		set<uint64_t> neighbors = it->second;
		print_set(neighbors);
		return pair<int, set<uint64_t> > (1, neighbors);
	}
	std::cout << "Node doesn't exist.  No neighbors to fetch." << std::endl;
	return pair<int, set<uint64_t> > (0, EmptySet); //is this correct?
}

//return pair (success/fail, distance/NULL)
//return (1, distance) if successful
//return (0, NoDistance) if there is no path
//return (2, NoDistance) if either node does not exist
pair<int, uint64_t> Graph::shortest_path(uint64_t node_a_id, uint64_t node_b_id){
	return pair<int, uint64_t> (2, NoDistance);
}



















