//greeter_server.h												Wendy Chen, 11/2016

//Contains #include, using, and functions for the greeter_server
//Based off grpc hello world example

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "helloworld.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::WriteRequest;
using helloworld::WriteAck;
using helloworld::Greeter;

#ifndef GREETER_SERVER_H
#define GREETER_SERVER_H

// Logic and data behind the server's behavior.
class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }

  Status SayHelloAgain(ServerContext *context, const HelloRequest* request,
		  HelloReply* reply) override {
    int command = request->command();

    std::cout << "Server gets command: " << command << std::endl;
    
    std::string prefix("Hello again ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }

  // rpc ForwardRequest (WriteRequest) return (WriteAck) {}
  // Status ForwardRequest(ServerContext* context, const WriteRequest* request,
  //                       WriteAck* reply) override {
  //   //in here, need to send HTTP Request.  using mongoose?
  //   std::string prefix("Write ");
  //   reply->set_message(prefix + request->name());
  // }
};

// void RunServer(void);
// void RunThreadedServer(void);

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}



// int main(int argc, char** argv);

#endif