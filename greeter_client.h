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
using helloworld::WriteRequest;
using helloworld::WriteAck;
using helloworld::Greeter;

#ifndef GREETER_CLIENT_H
#define GREETER_CLIENT_H

class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<Channel> channel)
  	: stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(const std::string& user);
  std::string SayHelloAgain(const std::string& user);
  std::string ForwardRequest(const std::string& user);
 private:
  std::unique_ptr<Greeter::Stub> stub_;
};

// int main(int argc, char** argv);

#endif