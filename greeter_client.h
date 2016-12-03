//greeter_client.h												Wendy Chen, 11/2016

//Contains #include, using, and functions for the greeter_client
//Based off grpc hello world example

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "helloworld.grpc.pb.h"
#include "stdint.h"

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
  std::string SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id, uint64_t server_node, uint64_t client_node); 
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};


//based on node's server rpc_port, 
//client has hardcoded node to send to
// int RunClient(unsigned int rpc_port, uint64_t command, uint64_t node_a_id, uint64_t node_b_id);
// int main(int argc, char** argv);


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

  return 0;
}

#endif