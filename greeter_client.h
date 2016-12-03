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
  std::string SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id);
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};


//based on node's server rpc_port, 
//client has hardcoded node to send to
int RunClient(unsigned int rpc_port, uint64_t command, uint64_t node_a_id, uint64_t node_b_id);
// int main(int argc, char** argv);

#endif