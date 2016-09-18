//graph.cpp 										Wendy Chen, 09/2016

//Implements an adjacency list, using a map to store
//(node, neighbor-list) pairs, where each neighbor list
//is vector<node>.  
//Class is defined in graph.h

#include "graph.h"

#include <iostream>
#include <cstdint> //has type 'uint64_t'
#include <utility> //has type 'pair'
#include <vector> //has type vector
#include <map>

using std::pair; 
using std::vector;
using std::map;

const vector<uint64_t> EmptyVector(0);
const uint64_t NoDistance = 0;

//Be consistent on what means success
// 1 is success
// 0 is fail
// 2 is extra option

////////////////////////////////////////////////////////////////////////
//return 1 if node is in the graph
//return 0 if node is not in the graph
int Graph::get_node(uint64_t node_id){
	std::cout << "Getting node: " << node_id << std::endl;
	map<uint64_t, vector<uint64_t> >::iterator it;
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
	map<uint64_t, vector<uint64_t> >::iterator it_a, it_b;
	it_a = this->nodes.find(node_a_id);
	it_b = this->nodes.find(node_b_id);
	if(it_a == this->nodes.end() || it_b == this->nodes.end()){
		std::cout << "One or both nodes do not exist" << std::endl;
		return 2;
	}
	//Nodes exist and have unique ids
	//Does the edge already exist?
	vector<uint64_t> &node_a_neighbors = it_a->second;
	vector<uint64_t> &node_b_neighbors = it_b->second;
	vector<uint64_t>::iterator it_a_neighbors;
	vector<uint64_t>::iterator it_b_neighbors;
	it_a_neighbors = find(node_a_neighbors.begin(), node_a_neighbors.end(), node_b_id);
	it_b_neighbors = find(node_b_neighbors.begin(), node_b_neighbors.end(), node_a_id);
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
		this->nodes.insert(pair<uint64_t, vector<uint64_t> >(node_id, EmptyVector));
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
		map<uint64_t, vector<uint64_t> >::iterator it_a, it_b;
		it_a = this->nodes.find(node_a_id);
		it_b = this->nodes.find(node_b_id);
		vector<uint64_t> &node_a_neighbors = it_a->second;
		vector<uint64_t> &node_b_neighbors = it_b->second;
		node_a_neighbors.push_back(node_b_id);
		node_b_neighbors.push_back(node_a_id);
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
	std::cout << "Client wants to remove edge (" << node_a_id << ", " << node_b_id << std::endl;
	//does edge exist?  Check neighbors of both nodes
	int found_edge = get_edge(node_a_id, node_b_id);
	if(found_edge == 1){
		//Edge is in graph.  Need to remove from both nodes.
		map<uint64_t, vector<uint64_t> >::iterator it_a, it_b;
		vector<uint64_t> it_a_neighbors = it_a.find(node_a_id)->second;
		vector<uint64_t> it_b_neighbors = it_b.find(node_b_id)->second;

		//Remove from each neighbor list.
		//IMPLEMENT THIS PART
		return 1;
	}
	//No edge to remove.
	return 0;
	
}



//return pair (success/fail, vector of neighbors)
//return (1, vector of neighbors) if node exists
//return (0, empty vector) if node doesn't exist
pair<int, vector<uint64_t> > Graph::get_neighbors(uint64_t node_id){
	return pair<int, vector<uint64_t> > (0, EmptyVector); //is this correct?
}

//return pair (success/fail, distance/NULL)
//return (1, distance) if successful
//return (0, NoDistance) if there is no path
//return (2, NoDistance) if either node does not exist
pair<int, uint64_t> Graph::shortest_path(uint64_t node_a_id, uint64_t node_b_id){
	return pair<int, uint64_t> (2, NoDistance);
}



















