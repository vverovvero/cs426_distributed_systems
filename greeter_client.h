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
  	std::string SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id) {
	  //Follows the same pattern as SayHello.
	  HelloRequest request;
	  request.set_command(command);
	  request.set_node_a_id(node_a_id);
	  request.set_node_b_id(node_b_id);
	  HelloReply reply;
	  ClientContext context;

	  //Here we can use the stub's newly available method we just added.
	  Status status = stub_->SayHelloAgain(&context, request, &reply);
	  if(status.ok()){
	    //Print out ack
	    std::string message("Request OK");
	    return message;
	  } else {
	    std::cout << status.error_code() << ": " << status.error_message()
	              << std::endl;
	    return "RPC failed";
	  }
	}
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};


//Send request to partition number
int RunClient(char * rpc_address, uint64_t command, uint64_t node_a_id, uint64_t node_b_id){
  std::string ipaddress(rpc_address);

  GreeterClient greeter(grpc::CreateChannel(
      ipaddress, grpc::InsecureChannelCredentials()));

  std::string reply = greeter.SayHelloAgain(command, node_a_id, node_b_id);
  std::cout << "Greeter received: " << reply << std::endl;

  //after receiving ack, ...
  if(command == 1){
    //if client asked for node existence
    unsigned int existence = reply->node_exists();
    printf("get_node for %d return %d\n", node_a_id, existence);
  }
  // else if(command == 2){
  //   //send response for add_edge(node_a_id, node_b_id)
  // }
  // else if(command == 3){
  //   //send response for remove_edge(node_a_id, node_b_id)
  // }

  return 0;
}

#endif