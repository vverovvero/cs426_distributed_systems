//greeter_client.h												Wendy Chen, 11/2016

//Contains #include, using, and functions for the greeter_client
//Based off grpc hello world example

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "helloworld.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;


#ifndef GREETER_CLIENT_H
#define GREETER_CLIENT_H

class Graph
{
	private:
		map<uint64_t, set<uint64_t> > nodes;
	public:
		int add_node(uint64_t node_id);
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