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


// Assambles the client's payload, sends it and presents the response back
// from the server.
// std::string GreeterClient::SayHello(const std::string& user) {
//   // Data we are sending to the server.
//   HelloRequest request;
//   request.set_name(user);

//   // Container for the data we expect from the server.
//   HelloReply reply;

//   // Context for the client. It could be used to convey extra information to
//   // the server and/or tweak certain RPC behaviors.
//   ClientContext context;

//   // The actual RPC.
//   Status status = stub_->SayHello(&context, request, &reply);

//   // Act upon its status.
//   if (status.ok()) {
//     return reply.message();
//   } else {
//     std::cout << status.error_code() << ": " << status.error_message()
//               << std::endl;
//     return "RPC failed";
//   }
// }

std::string GreeterClient::SayHelloAgain(uint64_t command, uint64_t node_a_id, uint64_t node_b_id) {
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

int RunClient(unsigned int rpc_port, uint64_t command, uint64_t node_a_id, uint64_t node_b_id){
  std::string ipaddress;
  if(rpc_port == 8080){
    ipaddress = "104.154.198.82:8090";
  }
  else if(rpc_port == 8090){
    ipaddress = "104.154.145.208:9000";
  }
  else if(rpc_port == 9000){
    std::cout << "Tail does not forward request" << std::endl;
    return 0;
  }
  else{
    std::cout << "Client failed" << std::endl;
    return -1;
  }

  GreeterClient greeter(grpc::CreateChannel(
      ipaddress, grpc::InsecureChannelCredentials()));

  std::string reply = greeter.SayHelloAgain(command, node_a_id, node_b_id);
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
    unsigned int rpc_port = (unsigned int) atoi(argv[1]);
    unsigned int command = (unsigned int) atoi(argv[2]);
    unsigned int node_a_id = (unsigned int) atoi(argv[3]);
    unsigned int node_b_id = (unsigned int) atoi(argv[4]);

    RunClient(rpc_port, command, node_a_id, node_b_id);
  }

  return 0;
}
