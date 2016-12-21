//greeter_server.h												Wendy Chen, 11/2016

//Contains #include, using, and functions for the greeter_server
//Based off grpc hello world example

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "helloworld.grpc.pb.h"
#include "stdint.h"
#include "stdlib.h"

#include "graph.h"
#include "globals.h"

#include "greeter_client.h" //will this be safe?

// Graph graph;

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

#ifndef GREETER_SERVER_H
#define GREETER_SERVER_H

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {

  Status SayHelloAgain(ServerContext *context, const HelloRequest* request,
		  HelloReply* reply) override {
    uint64_t command = request->command();
    uint64_t node_a_id = request->node_a_id();
    uint64_t node_b_id = request->node_b_id();
    uint64_t node_a_exists = request->node_a_exists();
    uint64_t node_b_exists = request->node_b_exists();
    uint64_t node_a_has_b = request->node_a_has_b();
    uint64_t node_b_has_a = request->node_b_has_a();

    //Server always executes what client asks for
    if(command == 1){
      //Check node_a existence
      unsigned int existence = graph.get_node(node_a_id);
      reply->set_node_exists(existence);
      // printf("server side, get_node for %lu, existence %u\n", node_a_id, existence);

      //Check for neighbor if node exists
      unsigned int has_neighbor = 0;
      if(existence == 1){
        pair<int, set<uint64_t> > result = graph.get_neighbors(node_a_id);
        if(result.first == 1){
          set<uint64_t> neighbors = result.second;
          // std::cout << "Found neighbors of: " << node_a_id << std::endl;
          // graph.print_set(neighbors);
          //Look through neighbors
          set<uint64_t>::iterator it;
          it = neighbors.find(node_b_id);
          if(it != neighbors.end()){
            has_neighbor = 1;
          }
        }
      }

      reply->set_has_neighbor(has_neighbor);
      // printf("server side, has_neighbor: %u\n", has_neighbor);
    }

    if(command == 2){
      // printf("server side, will ADD_EDGE\n");
      //lock and add edge
      partition_mtx.lock();
      graph.add_edge(node_a_id, node_b_id, node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);
      partition_mtx.unlock();

    }

    if(command == 3){
      //remove_edge
      // printf("server side, will REMOVE_EDGE\n");
      partition_mtx.lock();
      graph.remove_edge(node_a_id, node_b_id, node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);
      partition_mtx.unlock();
    }

    return Status::OK;
  }

};


void RunServer(char * rpc_server_port) {
  std::string server_address(rpc_server_port);
  GreeterServiceImpl service;
  
  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "grpc Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}




#endif