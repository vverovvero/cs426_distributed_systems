/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "greeter_client.h"
#include "helloworld.grpc.pb.h"
#include "stdint.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;


std::string GreeterClient::SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id, uint64_t server_node, uint64_t client_node) {
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

//rpc_port is that of the client
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


int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  // GreeterClient greeter(grpc::CreateChannel(
  //     "localhost:50051", grpc::InsecureChannelCredentials()));

  // std::string reply = greeter.SayHelloAgain(2, 666, 777);
  // std::cout << "Greeter received: " << reply << std::endl;

  if(argc == 5){
    std::cout << "greeter_client has enough arguments" << std::cout;
    unsigned int rpc_port = (unsigned int) atoi(argv[1]);
    unsigned int command = (unsigned int) atoi(argv[2]);
    unsigned int node_a_id = (unsigned int) atoi(argv[3]);
    unsigned int node_b_id = (unsigned int) atoi(argv[4]);

    std::cout << "rpc_port: " << rpc_port << std::cout;
    std::cout << "command: " << command << std::cout;
    std::cout << "node_a_id: " << node_a_id << std::cout;
    std::cout << "node_b_id: " << node_b_id << std::cout;

    RunClient(rpc_port, command, node_a_id, node_b_id);
  }

  return 0;
}
