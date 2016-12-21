//greeter_client.h												Wendy Chen, 11/2016

//Contains #include, using, and functions for the greeter_client
//Based off grpc hello world example

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "helloworld.grpc.pb.h"
#include "stdint.h"

#include "graph.h"
#include "globals.h"

#include <vector>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

#ifndef GREETER_CLIENT_H
#define GREETER_CLIENT_H

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
  	: stub_(Greeter::NewStub(channel)) {}
  	std::vector<unsigned int> SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a) {
	  //Follows the same pattern as SayHello.
	  HelloRequest request;
	  request.set_command(command);
	  request.set_node_a_id(node_a_id);
	  request.set_node_b_id(node_b_id);
    request.set_node_a_exists(node_a_exists);
    request.set_node_b_exists(node_b_exists);
    request.set_node_a_has_b(node_a_has_b);
    request.set_node_b_has_a(node_b_has_a);
	  HelloReply reply;
	  ClientContext context;

	  //Here we can use the stub's newly available method we just added.
	  Status status = stub_->SayHelloAgain(&context, request, &reply);
	  if(status.ok()){
      if(command == 1){
        unsigned int existence = reply.node_exists();
        unsigned int has_neighbor = reply.has_neighbor();
        std::vector<unsigned int> result = {0, existence, has_neighbor};
        return result;
      }
      //for command 2 or 3, always return 0
      std::vector<unsigned int> result = {0, 0, 0};
	    return result;
	  } 
    else {
	    std::cout << status.error_code() << ": " << status.error_message()
	              << std::endl;

      std::vector<unsigned int> result = {666, 666, 666};
	    return result;
	  }
	}
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};


//Send request to partition number
std::vector<unsigned int> RunClient(char * rpc_address, uint64_t command, uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a){
  std::string ipaddress(rpc_address);

  GreeterClient greeter(grpc::CreateChannel(
      ipaddress, grpc::InsecureChannelCredentials()));

  std::vector<unsigned int> reply = greeter.SayHelloAgain(command, node_a_id, node_b_id, node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);
  // std::cout << "Greeter received: " << reply[0] << reply[1] << reply[2] << std::endl;

  //after receiving ack, ...
  if(command == 1){
    //if client asked for node existence
    // printf("Client side, for node_id %lu, existence %u\n", node_a_id, reply[1]);
    // printf("Client side, has neighbor %u\n", reply[2]);
    // printf("Returned to Greeter after get_node\n");
    return reply;
  }
  //else, for command 2 or 3, return 0
  // printf("Returned to Greeter after add_edge or remove_edge");

  return reply;
}

#endif