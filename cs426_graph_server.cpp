//server.cpp                                                Wendy Chen, 09/2016

// Set up server to handle HTTP Requests.
// HTTP replies are found in api.h.
// Base code taken from mongoose simple_server and api example
// Copyright (c) 2015 Cesanta Software Limited
// All rights reserved

//multithreaded server
#include <omp.h>

#include "mongoose.h"
#include "api.h"
#include "graph.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h> //has atoll, has exit
#include <iostream>
#include <vector>
#include <utility> //has type 'pair'
#include <unistd.h> //getopt
#include <ctype.h> //isdigit

using std::pair;
using std::vector;

// static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

//static int s_sig_num = 0;
//static void *s_db_handle = NULL;
//static const char *s_db_path = "api_server.db";
static const struct mg_str s_post_method = MG_STR("POST");
static const struct mg_str key_add_node = MG_STR("/add_node");
static const struct mg_str key_add_edge = MG_STR("/add_edge");
static const struct mg_str key_remove_node = MG_STR("/remove_node");
static const struct mg_str key_remove_edge = MG_STR("/remove_edge");
static const struct mg_str key_get_node = MG_STR("/get_node");
static const struct mg_str key_get_edge = MG_STR("/get_edge");
static const struct mg_str key_get_neighbors = MG_STR("/get_neighbors");
static const struct mg_str key_shortest_path = MG_STR("/shortest_path");

static const vector<uint64_t> EmptyVector; //if no node id's found

Graph graph; //global graph
static const char *s_http_port = "666"; //global port
unsigned int ipaddress = 666; //global ipaddress (for RPC)

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
              event_add_node(&graph, nc, result.second[0]);
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_add_edge)){
            // print_flush("KEY was add_edge");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 2, "node_a_id", "node_b_id");
            if(result.first == 1) {
              event_add_edge(&graph, nc, result.second[0], result.second[1]);
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_remove_node)){
            // print_flush("KEY was remove_node");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 1, "node_id");
            if(result.first == 1) {
              event_remove_node(&graph, nc, result.second[0]);
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_remove_edge)){
            // print_flush("KEY was remove_edge");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 2, "node_a_id", "node_b_id");
            if(result.first == 1) {
              event_remove_edge(&graph, nc, result.second[0], result.second[1]);
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_get_node)){
            // print_flush("KEY was get_node");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 1, "node_id");
            if(result.first == 1) {
              event_get_node(&graph, nc, result.second[0]);
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_get_edge)){
            // print_flush("KEY was get_edge");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 2, "node_a_id", "node_b_id");
            if(result.first == 1) {
              event_get_edge(&graph, nc, result.second[0], result.second[1]);
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_get_neighbors)){
            // print_flush("KEY was get_neighbors");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 1, "node_id");
            if(result.first == 1) {
              event_get_neighbors(&graph, nc, result.second[0]);
            }
            else {
              error = 1;
            }
          }
          else if(is_equal(&key, &key_shortest_path)){
            // print_flush("KEY was shortest_path");
            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 2, "node_a_id", "node_b_id");
            if(result.first == 1) {
              event_shortest_path(&graph, nc, result.second[0], result.second[1]);
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


int main(int argc, char *argv[]) {
  //Need at least port and devfile
  if(argc >= 2){
    //Fetch all the arguments
    //If format flag, then the format option flag must come first
    int c;
    int b_flag = 0;
    // static const char *s_http_port;
    while(optind < argc){
      if((c = getopt(argc, argv, "b:")) != -1){
        switch (c) {
          case 'b':
            //set format flag
            // printf("Found format flag\n");
            b_flag = 1;
            ipaddress = atoi(optarg);
            break;
          default:
            break;
        }
      }
      else{
        while(optind < argc){
           //Handle regular arguments
          s_http_port = argv[optind];
          optind++;
        }
        break;
      }
    }
      
    //Sanity check the arguments
    printf("port: %s\n", s_http_port);
    printf("ipaddress (from -b): %u\n", ipaddress);

    //Split into two threads, one for each server
    static int tid;
    #pragma omp threadprivate(tid)

    omp_set_num_threads(2);
    #pragma omp parallel
    {
      tid = omp_get_thread_num();
      printf("Hello world! from thread %d of %d\n", tid, omp_get_num_threads());
      if(tid == 0){
        printf("thread id %d will run the rpc server\n", omp_get_thread_num());
      }
      else{
        printf("thread id %d will run the mongoose http server\n", omp_get_thread_num());
      }

    }

    ////////////////////////////////////////////////
    //Set up the RPC server

    ////////////////////////////////////////////////
    //Set up the http server
    struct mg_mgr mgr;
    struct mg_connection *nc;

    mg_mgr_init(&mgr, NULL);
    nc = mg_bind(&mgr, s_http_port, ev_handler);

    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = ".";      // Serve current directory
    s_http_server_opts.dav_document_root = ".";  // Allow access via WebDav
    s_http_server_opts.enable_directory_listing = "yes";

    printf("Starting web server on port %s\n", s_http_port);
    for (;;) {
      mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

    return 0;
  }
  else{
    std::cout << "Specify port by : './cs426_graph_server <optional -b ipaddress> <port> '" << std::endl;
  }
}
