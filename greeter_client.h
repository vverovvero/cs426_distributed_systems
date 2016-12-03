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

// Graph graph;

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
  // std::string SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id, uint64_t server_node, uint64_t client_node); 
  	std::string SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id, uint64_t server_node, uint64_t client_node) {
	  //Follows the same pattern as SayHello.
	  HelloRequest request;
	  request.set_command(command);
	  request.set_node_a_id(node_a_id);
	  request.set_node_b_id(node_b_id);
	  request.set_server_node(server_node);
	  request.set_client_node(client_node);
	  HelloReply reply;
	  ClientContext context;

	  //Here we can use the stub's newly available method we just added.
	  Status status = stub_->SayHelloAgain(&context, request, &reply);
	  if(status.ok()){
	    //edit the graph if status was ok
	    if(command == 1){
	      //add node
	      std::cout << "Received ack!  Client should add node: " << node_a_id << std::endl;
	    }
	    else if(command == 2){
	      //add edge
	      std::cout << "Received ack!  Client should add edge: " << node_a_id << " " << node_b_id << std::endl;
	    }
	    else if(command == 3){
	      //remove node
	      std::cout << "Received ack!  Client should remove node: " << node_a_id << std::endl;
	    }
	    else if (command == 4){
	      //remove edge
	      std::cout << "Received ack!  Client should remove edge: " << node_a_id << " " << node_b_id << std::endl;
	    }
	    else{
	      std::cout << "Received ack!  Client has faulty command "<< std::endl;
	    }

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


//based on node's server rpc_port, 
//client has hardcoded node to send to



int RunClient(unsigned int rpc_port, uint64_t command, uint64_t node_a_id, uint64_t node_b_id){
  std::string ipaddress;
  uint64_t server_node = 0;
  uint64_t client_node = 0;
  if(rpc_port == 8080){
    client_node = 8080;
    server_node = 8090;
    ipaddress = "104.154.198.82:8090";
    std::cout << "Forward request to: " << ipaddress << std::endl;
  }
  else if(rpc_port == 8090){
    client_node = 8090;
    server_node = 9000;
    ipaddress = "104.154.145.208:9000";
    std::cout << "Forward request to: " << ipaddress << std::endl;
  }
  else if(rpc_port == 9000){
    client_node = 9000;
    std::cout << "Tail does not forward request" << std::endl;
    //tail modifies the graph
    std::cout << "Tail client modifies the graph here" << std::endl;
	if(command == 1){
		std::cout << "Graph add node!" << std::endl;
    	graph.add_node(node_a_id);
    }
    else if(command == 2){
      	std::cout << "Graph add edge!" << std::endl;
    	graph.add_edge(node_a_id, node_b_id);
    }
    else if(command == 3){
      	std::cout << "Graph remove node!" << std::endl;
    	graph.remove_node(node_a_id);
    }
    else if (command == 4){
      	std::cout << "Graph remove edge!" << std::endl;
    	graph.remove_edge(node_a_id, node_b_id);
    }
    else{
      std::cout << "Graph faulty command "<< std::endl;
    }
    return 0;
  }
  else{
    std::cout << "Client failed" << std::endl;
    return -1;
  }

  GreeterClient greeter(grpc::CreateChannel(
      ipaddress, grpc::InsecureChannelCredentials()));

  std::string reply = greeter.SayHelloAgain(command, node_a_id, node_b_id, server_node, client_node);
  std::cout << "Greeter received: " << reply << std::endl;

  //after receiving ack, middle men clients can modify graph
  std::cout << "Client modifies the graph here" << std::endl;
  	if(command == 1){
		std::cout << "Graph add node!" << std::endl;
    	graph.add_node(node_a_id);
    }
    else if(command == 2){
      	std::cout << "Graph add edge!" << std::endl;
    	graph.add_edge(node_a_id, node_b_id);
    }
    else if(command == 3){
      	std::cout << "Graph remove node!" << std::endl;
    	graph.remove_node(node_a_id);
    }
    else if (command == 4){
      	std::cout << "Graph remove edge!" << std::endl;
    	graph.remove_edge(node_a_id, node_b_id);
    }
    else{
      std::cout << "Graph faulty command "<< std::endl;
    }
  return 0;
}

#endif