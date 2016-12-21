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

#include <mutex>  

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
//init functions
void Graph::init_partition_info(unsigned int partition_no, unsigned int partition_total){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}
	//Update private variables
	this->partition_no = partition_no;
	this->partition_total = partition_total;

	//Unlock if most external scope
	if(memory_bit){
		this->graph_mtx.unlock();
		this->is_locked = false;
	}
}

//////////////////////////////////////////////////////////////////////
//Helper functions

//prints contents of a set
void Graph::print_set(set<uint64_t> neighbors){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}
	std::cout << "Printing set:" << std::endl;
	for(set<uint64_t>::iterator i = neighbors.begin(); i != neighbors.end(); i++){
		uint64_t neighbor = *i;
		std::cout << neighbor << std::endl;
	}
	//Unlock if most external scope
	if(memory_bit){
		this->graph_mtx.unlock();
		this->is_locked = false;
	}
}

////////////////////////////////////////////////////////////////////////
//return 1 if node is in the graph
//return 0 if node is not in the graph
//return 2 if partition error
int Graph::get_node(uint64_t node_id){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}

	//Check partition
	if(((node_id % this->partition_total) + 1) != this->partition_no){
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}

	// std::cout << "Getting node: " << node_id << std::endl;
	map<uint64_t, set<uint64_t> >::iterator it;
	it = this->nodes.find(node_id);
	if(it != this->nodes.end()){
		//Found the node
		// std::cout << "Found the node " << it->first << std::endl;
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 1;
	}
	//Node was not found
	// std::cout << "Node was not found" << std::endl;
	//Unlock if most external scope
	if(memory_bit){
		this->graph_mtx.unlock();
		this->is_locked = false;
	}
	return 0;
}

//return 1 if edge is in graph
//return 0 if edge is not in graph
//return 2 if at least one of vertices does not exist (or if nodes are same?)
//if either node_a_id or node_b_id exists on partition, then get_edge should return the correct answer
int Graph::get_edge(uint64_t node_a_id, uint64_t node_b_id){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}

	//Partition check (both nodes do not exist on this partition)
	if( (((node_a_id % this->partition_total) + 1) != this->partition_no) && 
	   	(((node_b_id % this->partition_total) + 1) != this->partition_no) ){
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}

	// std::cout << "Getting edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	//are the nodes the same?
	if(node_a_id == node_b_id){
		// std::cout << "Nodes are the same" << std::endl;
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}
	//do the nodes exist and belong to this partition?
	map<uint64_t, set<uint64_t> >::iterator it_a, it_b;
	it_a = this->nodes.find(node_a_id);
	it_b = this->nodes.find(node_b_id);
	if(((it_a == this->nodes.end()) &&  (((node_a_id % this->partition_total) + 1) == this->partition_no) ) || 
		((it_b == this->nodes.end()) && (((node_b_id % this->partition_total) + 1) == this->partition_no) ) ){
		// std::cout << "One or both nodes do not exist" << std::endl;
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}
	//Nodes exist and have unique ids
	//Does the edge already exist? and node belong on the partition?
	set<uint64_t> &node_a_neighbors = it_a->second;
	set<uint64_t> &node_b_neighbors = it_b->second;
	set<uint64_t>::iterator it_a_neighbors;
	set<uint64_t>::iterator it_b_neighbors;
	it_a_neighbors = node_a_neighbors.find(node_b_id);
	it_b_neighbors = node_b_neighbors.find(node_a_id);
	if(((it_a_neighbors != node_a_neighbors.end()) && (((node_a_id % this->partition_total) + 1) == this->partition_no) ) || 
	   ((it_b_neighbors != node_b_neighbors.end()) && (((node_b_id % this->partition_total) + 1) == this->partition_no) ) ){
		//Edge was found.  Don't add it.
		// std::cout << "Edge was found (on both nodes)." << std::endl;
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 1;
	}
	
	//Edge doesn't exist, add it
	// std::cout << "Edge doesn't exist.  Was not found." << std::endl;
	//Unlock if most external scope
	if(memory_bit){
		this->graph_mtx.unlock();
		this->is_locked = false;
	}
	return 0;
}

//return 1 if success, 0 if node already exists
//return 2 if partition error
int Graph::add_node(uint64_t node_id){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}

	//Check partition
	if(((node_id % this->partition_total) + 1) != this->partition_no){
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}

	// std::cout << "Client would like to add node: " << node_id << std::endl;
	if(get_node(node_id) == 0){
		//Node was not found, create it
		// std::cout << "Adding node with id: " << node_id << std::endl;
		this->nodes.insert(pair<uint64_t, set<uint64_t> >(node_id, EmptySet));
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 1;
	}
	//Node already exists
	// std::cout << "Cannot add node that already exists" << std::endl;
	//Unlock if most external scope
	if(memory_bit){
		this->graph_mtx.unlock();
		this->is_locked = false;
	}
	return 0;

}

//REQUIRES RPC MODIFICATION
//return 1 if success, 0 if edge already exists,
//2 if node doesn't exist or node_a_id == node_b_id
//add edge for the node if it exists on the partition.  else, rely on RPC to call add_edge on another partition.
int Graph::add_edge(uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}

	//Check partition
	if( (((node_a_id % this->partition_total) + 1) != this->partition_no) && 
	   	(((node_b_id % this->partition_total) + 1) != this->partition_no) ){
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}

	std::cout << "Client wants to add edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	if((node_a_exists == 0) || (node_b_exists == 0) || (node_a_id == node_b_id)){
		std::cout << "Cannot add edge because node(s) doesn't exist, or is same." << std::endl;
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}
	else if((node_a_has_b == 1) && (node_b_has_a == 1)){
		std::cout << "Edge already in graph.  Do not add." << std::endl;
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 0;
	}
	else{
		std::cout << "Adding edge!" << std::endl;
		map<uint64_t, set<uint64_t> >::iterator it_a, it_b;
		//add_edge for node_a_id
		if(((node_a_id % this->partition_total) + 1) == this->partition_no){
			it_a = this->nodes.find(node_a_id);
			set<uint64_t> &node_a_neighbors = it_a->second;
			node_a_neighbors.insert(node_b_id);
		}
		
		//add_edge for node_b_id
		if(((node_b_id % this->partition_total) + 1) == this->partition_no){
			it_b = this->nodes.find(node_b_id);
			set<uint64_t> &node_b_neighbors = it_b->second;
			node_b_neighbors.insert(node_a_id);
		}
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 1;
	}
}


//REQUIRES RPC MODIFICATION
//return 1 if success, 0 if edge does not exist
int Graph::remove_edge(uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}

	//if one of the nodes doesn't exist
	if((node_a_exists == 0) || (node_b_exists == 0)){
		std::cout << "One of the nodes doesn't exist" << std::endl;
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return 2;
	}

	std::cout << "Client wants to remove edge (" << node_a_id << ", " << node_b_id << ")." << std::endl;
	//does edge exist?  Check neighbors of both nodes
	if((node_a_has_b == 1) && (node_b_has_a == 1)){
		std::cout << "Removing edge!" << std::endl;
		//Edge is in graph.  Get neighbors for each node, and remove the opposite 
		if(((node_a_id % this->partition_total) + 1) == this->partition_no){
			this->nodes[node_a_id].erase(node_b_id);
		}

		if(((node_b_id % this->partition_total) + 1) == this->partition_no){
			this->nodes[node_b_id].erase(node_a_id);
		}

		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}

		return 1;
	}
	//No edge to remove.
	std::cout << "No edge to remove." << std::endl;
	//Unlock if most external scope
	if(memory_bit){
		this->graph_mtx.unlock();
		this->is_locked = false;
	}
	return 0;
}


//return pair (success/fail, vector of neighbors)
//return (1, set of neighbors) if node exists
//return (0, empty set) if node doesn't exist
//return (2, empty set) if not in partition
pair<int, set<uint64_t> > Graph::get_neighbors(uint64_t node_id){
	//Lock if most external scope
	int memory_bit = 0;
	if(!(this->is_locked)){
		this->graph_mtx.lock();
		this->is_locked = true;
		memory_bit = 1;
	}

	//Check partition
	if(((node_id % this->partition_total) + 1) != this->partition_no){
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return pair<int, set<uint64_t> > (2, EmptySet);;
	}

	// std::cout << "Client would like to get neighbors of node: " << node_id << std::endl;
	//first find node
	int found_node = get_node(node_id);
	if(found_node == 1){
		//Node found.  Return the set of neighbors.
		//Unlock if most external scope
		if(memory_bit){
			this->graph_mtx.unlock();
			this->is_locked = false;
		}
		return pair<int, set<uint64_t> > (1, this->nodes[node_id]);
	}
	// std::cout << "Node doesn't exist.  No neighbors to fetch." << std::endl;
	//Unlock if most external scope
	if(memory_bit){
		this->graph_mtx.unlock();
		this->is_locked = false;
	}
	return pair<int, set<uint64_t> > (0, EmptySet); //is this correct?
}

