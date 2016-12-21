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

    //Server always executes what client asks for
    if(command == 1){
      //Check node_a existence
      unsigned int existence = graph.get_node(node_a_id);
      reply->set_node_exists(existence);
      printf("server side, get_node for %lu, existence %u\n", node_a_id, existence);
    }

    return Status::OK;

    // //when server receives client request, server either forwards or acks
    // if(server_node == 9000){
    //   // std::cout << "Server is tail, send ack" << std::endl;
    //   //server modifies the graph here
    //   // std::cout << "Tail server should modify the graph" << std::endl;
    //   if(command == 1){
    //     // std::cout << "Graph add node!" << std::endl;
    //     graph.add_node(node_a_id);
    //   }
    //   else if(command == 2){
    //       // std::cout << "Graph add edge!" << std::endl;
    //     graph.add_edge(node_a_id, node_b_id);
    //   }
    //   else if(command == 3){
    //       // std::cout << "Graph remove node!" << std::endl;
    //     // graph.remove_node(node_a_id);
    //   }
    //   else if (command == 4){
    //       // std::cout << "Graph remove edge!" << std::endl;
    //     graph.remove_edge(node_a_id, node_b_id);
    //   }
    //   else{
    //     // std::cout << "Graph faulty command "<< std::endl;
    //   }
    //   return Status::OK;
    // }
    // else{
    //   // std::cout << "Server is not tail, forward client request" << std::endl;
    //   //forward client request
    //   if(RunClient(server_node, command, node_a_id, node_b_id) == 0){
    //     //when client request returns successfully, server sends ack
    //     return Status::OK;
    //   }
    // }
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