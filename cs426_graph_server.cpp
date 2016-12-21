//server.cpp                                                Wendy Chen, 09/2016

// Set up server to handle HTTP Requests.
// HTTP replies are found in api.h.
// Base code taken from mongoose simple_server and api example
// Copyright (c) 2015 Cesanta Software Limited
// All rights reserved

#include "mongoose.h"
#include "api.h"
#include "graph.h"
#include "globals.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h> //has atoll, has exit
#include <iostream>
#include <vector>
#include <utility> //has type 'pair'
#include <unistd.h> //getopt
#include <ctype.h> //isdigit
#include <thread> //multithreaded server

// #include <iostream>
#include <memory>
#include <string>
#include <grpc++/grpc++.h>
#include "helloworld.grpc.pb.h"
#include "greeter_server.h"
#include "greeter_client.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using helloworld::HelloRequest;
using helloworld::HelloReply;
using helloworld::Greeter;

using std::pair;
using std::vector;
using std::thread;

static struct mg_serve_http_opts s_http_server_opts;

static const struct mg_str s_post_method = MG_STR("POST");
static const struct mg_str key_add_node = MG_STR("/add_node");
static const struct mg_str key_add_edge = MG_STR("/add_edge");
static const struct mg_str key_remove_edge = MG_STR("/remove_edge");
static const struct mg_str key_get_node = MG_STR("/get_node");
static const struct mg_str key_get_edge = MG_STR("/get_edge");
static const struct mg_str key_get_neighbors = MG_STR("/get_neighbors");

static const vector<uint64_t> EmptyVector; //if no node id's found

static const char *s_http_port = "666"; //global http port
unsigned int ipaddress = 666; //global ipaddress (for RPC) //don't want this anymore
// static const char *rpc_server_port = "667"; //global rpc port
static const char *ipaddress_rpc_port = "100000.667"; //for running the rpc server
unsigned int partition_total = 0;
unsigned int partition_no = 0;

//////// my helper print functions////////////
void print_flush(char * string){
  printf("%s\n", string);
  fflush(stdout);
}

void print_mg_str(struct mg_str string){
  int i;
  for(i=0;i<string.len;i++){
    //print character with offset i
    printf("%c", *(string.p + i));
  }
  printf("\n");
  fflush(stdout);
}

void print_json_token(struct json_token * token){
  printf("json num_desc: %d\n", token->num_desc);

  int i;
  for(i=0;i<token->len;i++){
    printf("%c", *(token->ptr + i));
  }
  printf("\n");
  fflush(stdout);
}

////////////////////////////////////////////////////////
//for fetching node id's and returning them as a uint64_t
uint64_t token_to_uint64(struct json_token * token){
  uint64_t value = atoll(token->ptr); //convert char to long long
  return value;
}

//given http request body, return (1, vector of node ids) on successful parse
//return (0, empty vector) if no node ids found
pair<int, vector<uint64_t> > parse_for_node_ids(struct mg_str body, int num, ...){
  int i;
  vector<uint64_t> node_ids;
  //set up for multiple node ids
  va_list valist;
  va_start(valist, num);

  //Get json_tokens from body
  struct json_token *tokens = parse_json2(body.p, body.len);
  // print_json_token(tokens);

  //Find specified node ids
  for(i=0;i<num;i++){
    const char * node_id_path = va_arg(valist, const char *);
    //this will SEGFAULT if node_id_path is not found.  bad?
    struct json_token *token = find_json_token(tokens, node_id_path);
    //print_json_token(token);
    if(token !=0){ //only convert token to uint64_t if it was found
      uint64_t node_id = token_to_uint64(token);

      //push result into vector
      node_ids.push_back(node_id);
    }
  }

  //clean up valist
  va_end(valist);

  if(node_ids.size() > 0){
    return pair<int, vector<uint64_t> > (1, node_ids);
  }
  else {
    return pair<int, vector<uint64_t> > (0, EmptyVector);
  }
}

/////////////////////////////////////////////
static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
  return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
  return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  static const struct mg_str api_prefix = MG_STR("/api/v1");
  struct http_message *hm = (struct http_message *) ev_data;
  struct mg_str key;
  int error = 0;

  //memory buffer issue?
  struct mbuf *io_send = &nc->send_mbuf;

  //clear the send buf first?
  mbuf_remove(io_send, io_send->len);

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (has_prefix(&hm->uri, &api_prefix)) {
        key.p = hm->uri.p + api_prefix.len;
        key.len = hm->uri.len - api_prefix.len;

        if (is_equal(&hm->method, &s_post_method)) {
          //Parse the desired command by checking the key
          if(is_equal(&key, &key_add_node)){
            // print_flush("KEY was add_node");
            //Parse Json
            //Call wrapper, which edits graph and returns HTTP reply
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 1, "node_id");
            if(result.first == 1) {
              partition_mtx.lock();
              event_add_node(&graph, nc, result.second[0]);
              partition_mtx.unlock();
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_add_edge)){
            // print_flush("KEY was add_edge");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 2, "node_a_id", "node_b_id");
            if(result.first == 1) {
              //Lock partition
              partition_mtx.lock();
              // //SEND RPC REQUEST for node existence
              unsigned int node_a_id = result.second[0];
              unsigned int node_b_id = result.second[1];
              unsigned int partition_a_no = (node_a_id % partition_total) + 1;
              unsigned int partition_b_no = (node_b_id % partition_total) + 1;

              std::vector<unsigned int> result_a = RunClient(partition[partition_a_no], 1, node_a_id, node_b_id, 0, 0, 0, 0);
              unsigned int node_a_exists = result_a[1];
              unsigned int node_a_has_b = result_a[2];

              std::vector<unsigned int> result_b = RunClient(partition[partition_b_no], 1, node_b_id, node_a_id, 0, 0, 0, 0);
              unsigned int node_b_exists = result_b[1];
              unsigned int node_b_has_a = result_b[2];

              //Now, make rpc call to high partition number
              unsigned int higher_partition;
              (partition_a_no > partition_b_no) ? (higher_partition = partition_a_no) : (higher_partition = partition_b_no);

              // printf("Send rpc to higher_partition: %u\n", higher_partition);

              RunClient(partition[higher_partition], 2, node_a_id, node_b_id, node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);

              // printf("From http server side, node_a_id %u existence %u\n", node_a_id, node_a_exists);
              // printf("From http server side, node_b_id %u existence %u\n", node_b_id, node_b_exists);
              // printf("Safe to call graph add edge...\n");

              
              //Make local change on partition
              event_add_edge(&graph, nc, result.second[0], result.second[1], node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);
              //Unlock
              partition_mtx.unlock();
              
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_remove_edge)){
            // print_flush("KEY was remove_edge");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 2, "node_a_id", "node_b_id");
            if(result.first == 1) {
              //Lock partition
              partition_mtx.lock();
              //SEND RPC REQUEST
              unsigned int node_a_id = result.second[0];
              unsigned int node_b_id = result.second[1];
              unsigned int partition_a_no = (node_a_id % partition_total) + 1;
              unsigned int partition_b_no = (node_b_id % partition_total) + 1;

              std::vector<unsigned int> result_a = RunClient(partition[partition_a_no], 1, node_a_id, node_b_id, 0, 0, 0, 0);
              unsigned int node_a_exists = result_a[1];
              unsigned int node_a_has_b = result_a[2];

              std::vector<unsigned int> result_b = RunClient(partition[partition_b_no], 1, node_b_id, node_a_id, 0, 0, 0, 0);
              unsigned int node_b_exists = result_b[1];
              unsigned int node_b_has_a = result_b[2];

              //Now, make rpc call to high partition number
              unsigned int higher_partition;
              (partition_a_no > partition_b_no) ? (higher_partition = partition_a_no) : (higher_partition = partition_b_no);

              // printf("Send rpc to higher_partition: %u\n", higher_partition);


              RunClient(partition[higher_partition], 3, node_a_id, node_b_id, node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);

              // printf("From http server side, node_a_id %u existence %u\n", node_a_id, node_a_exists);
              // printf("From http server side, node_b_id %u existence %u\n", node_b_id, node_b_exists);
              // printf("Safe to call graph remove edge...\n");

              //Make local change on partition
              event_remove_edge(&graph, nc, result.second[0], result.second[1], node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);
              
              //Unlock
              partition_mtx.unlock();         
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_get_node)){
            // print_flush("KEY was get_node");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 1, "node_id");
            if(result.first == 1) {
              partition_mtx.lock();
              event_get_node(&graph, nc, result.second[0]);
              partition_mtx.unlock();
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_get_edge)){
            // print_flush("KEY was get_edge");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 2, "node_a_id", "node_b_id");
            if(result.first == 1) {
              partition_mtx.lock();
              event_get_edge(&graph, nc, result.second[0], result.second[1]);
              partition_mtx.unlock();
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_get_neighbors)){
            // print_flush("KEY was get_neighbors");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 1, "node_id");
            if(result.first == 1) {
              partition_mtx.lock();
              event_get_neighbors(&graph, nc, result.second[0]);
              partition_mtx.unlock();
            }
            else {
              error = 1;
            }
          }
          else{
            // print_flush("KEY was not recognized!");
            mg_send_head(nc, 400, -1, NULL);
            mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "Bad Request\n");
            mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
          }
          if(error == 1){
            // print_flush("A POST KEY command failed...");
            mg_send_head(nc, 500, -1, NULL);
            mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 500, "Server Error\n");
            mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
          }
        } 
        else {
          // print_flush("Request was NOT a POST request");
          //reply
          mg_send_head(nc, 200, -1, NULL);
          mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 500, "Error Not Implemented\n");
          mg_send_http_chunk(nc, "", 0); // Tell the client we're finished

        }
      } else {
        // printf("Received unrecognized request...\n");
        // fflush(stdout);
        // mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */

        mg_send_head(nc, 400, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "Unrecognized Client Request\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
      }
      break;
    default:
      break;
  }
}


///////////helper multithreaded server functions//////////////
void serve_rpc(){
  std::cout << "Hello, World from rpc server" << std::endl;
  // RunThreadedServer();
  RunServer(partition[partition_no]);
}



/////////////////////////////////////////
int main(int argc, char *argv[]) {
  //Need at least executable and port
  if((argc >= 2) && (argc < 16)){
    //Fetch all the arguments
    //If format flag, then the format option flag must come first
    int c;
    int b_flag = 0;
    // static const char *s_http_port;
    while(optind < argc){
      if((c = getopt(argc, argv, "p:l:")) != -1){
        switch (c) {
          case 'p':
            //GET PART NUMBER
            partition_no = atoi(optarg);
            break;
          case 'l':
            //GET First item in partition list
            partition_total++;
            if(partition_no == partition_total){
              ipaddress_rpc_port = optarg;
            }
            partition[partition_total] = optarg;
            break;
          default:
            break;
        }
      }
      else{
        while(optind < argc){
           //Handle regular arguments
          if(optind == 5){
            s_http_port = argv[optind];
            optind++;
          }
          //Grab the rest of the partitions
          else{
            partition_total++;
            if(partition_no == partition_total){
              ipaddress_rpc_port = argv[optind];
            }
            partition[partition_total] = argv[optind];
            optind++;
          }  
        }
        break;
      }
    }
      
    //Sanitize the address in partition[partition_no]
    //Sanitize server address
    const char s[2] = ":";
    char *token, *last;
    token = strtok(partition[partition_no], s);
     
    /* walk through other tokens */
    // printf("[D] printing tokens: \n");
    while( token != NULL ) 
    {
      // printf( "[D] %s\n", token );

      last = token;
      token = strtok(NULL, s);
    }

    char address[100];
    strcpy (address,"0.0.0.0:");
    strcat (address, last);
    partition[partition_no] = address;

    //Sanity check the arguments////////////////////
    printf("s_http_port: %s\n", s_http_port);
    printf("partition_no: %u\n", partition_no);
    printf("partition_total: %u\n", partition_total);
    // printf("ipaddress: %s\n", ipaddress);
    // printf("rpc_server_port: %s\n", rpc_server_port);;
    printf("ipaddress_rpc_port: %s\n", ipaddress_rpc_port);

    for(int j=1; j<=partition_total; j++){
      printf("%d\t%s\n", j, partition[j]);
    }

    ////////////////////////////////////////////////
    //Give graph the partition info
    graph.init_partition_info(partition_no, partition_total);

    ////////////////////////////////////////////////
    //Launch a thread for the rpc server
    thread t1(serve_rpc); //commented out for now

    ////////////////////////////////////////////////
    // Set up the http server
    struct mg_mgr mgr;
    struct mg_connection *nc;

    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, s_http_port, ev_handler);

    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = ".";      // Serve current directory
    s_http_server_opts.dav_document_root = ".";  // Allow access via WebDav
    s_http_server_opts.enable_directory_listing = "yes";

    printf("Starting mongoose http web server on port %s\n", s_http_port);
    for (;;) {
      mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
    //join the thread
    t1.join(); //commented out for now
  }
  else{
    std::cout << "Usage: ./cs426_graph_server <graph_server_port> -p <partnum> -l <partlist> " << std::endl;
  }
  return 0;
}
