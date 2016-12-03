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

#include "greeter_client.h" //will this be safe?
#include "greeter_client.cc"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

#ifndef GREETER_SERVER_H
#define GREETER_SERVER_H

// std::string GreeterClient::SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id, uint64_t server_node, uint64_t client_node);
// int RunClient(unsigned int rpc_port, uint64_t command, uint64_t node_a_id, uint64_t node_b_id);


// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {

  Status SayHelloAgain(ServerContext *context, const HelloRequest* request,
		  HelloReply* reply) override {
    uint64_t command = request->command();
    uint64_t node_a_id = request->node_a_id();
    uint64_t node_b_id = request->node_b_id();
    uint64_t server_node = request->server_node();
    uint64_t client_node = request->client_node();

    // std::cout << "Server gets command: " << command << std::endl;
    // if(command == 1){
    //   std::cout << "Server should add node: " << node_a_id << std::endl;
    // }
    // else if(command == 2){
    //   std::cout << "Server should add edge: " << node_a_id << " " << node_b_id << std::endl;
    // }
    // else if(command == 3){
    //   std::cout << "Server should remove node: " << node_a_id << std::endl;
    // }
    // else if (command == 4){
    //   std::cout << "Server should remove edge: " << node_a_id << " " << node_b_id << std::endl;
    // }
    // else{
    //   std::cout << "Server received faulty command" << std::endl; 
    // }

    //when server receives client request, server either forwards or acks
    if(server_node == 9000){
      std::cout << "Server is tail, send ack" << std::endl;
      return Status::OK;
    }
    else{
      std::cout << "Server is not tail, forward client request" << std::endl;
      //forward client request
      if(RunClient(server_node, command, node_a_id, node_b_id) == 0){
        //when client request returns successfully, server sends ack
        return Status::OK;
      }
    }

    // return Status::OK;
  }

};


void RunServer(unsigned int rpc_port) {
  // std::string server_address("0.0.0.0:50051");
  std::string server_address;
  GreeterServiceImpl service;
  if(rpc_port == 8080){
    // server_address = "104.198.165.44:8080";
    server_address = "0.0.0.0:8080";
  }
  else if(rpc_port == 8090){
    // server_address = "104.154.198.82:8090";
    server_address = "0.0.0.0:8090";
  }
  else if(rpc_port == 9000){
    // server_address = "104.154.145.208:9000";
    server_address = "0.0.0.0:9000";
  }
  else{
    std::cout << "Failed to start grpc server." << std::endl;
    return;
  }
  
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



// int main(int argc, char** argv);

#endif