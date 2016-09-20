//server.cpp                                                Wendy Chen, 09/2016

// Set up server to handle HTTP Requests.
// HTTP replies are found in api.h.
// Base code taken from mongoose simple_server and api example
// Copyright (c) 2015 Cesanta Software Limited
// All rights reserved

#include "mongoose.h"
#include "api.h"
#include "graph.h"

#include <stdarg.h>
#include <iostream>
#include <cstdint> //has type 'uint64_t'
#include <stdlib.h> //has atoll
#include <vector>
#include <utility> //has type 'pair'

using std::pair;
using std::vector;

static const char *s_http_port = "8000";
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
  print_json_token(tokens);

  //Find specified node ids
  for(i=0;i<num;i++){
    const char * node_id_path = va_arg(valist, const char *);
    struct json_token *token = find_json_token(tokens, node_id_path);
    print_json_token(token);
    uint64_t node_id = token_to_uint64(token);
    printf("Node_id found was: %llu\n", node_id);
    fflush(stdout);

    //push result into vector
    node_ids.push_back(node_id);
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

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
      if (has_prefix(&hm->uri, &api_prefix)) {
        key.p = hm->uri.p + api_prefix.len;
        key.len = hm->uri.len - api_prefix.len;

        printf("Received a valid request\n");
        fflush(stdout);
        // print_flush("~~~~~KEY~~~~~");
        // print_mg_str(key);
        // print_flush("~~~~~HM-MESSAGE~~~~~");
        // print_mg_str(hm->message);
        // print_flush("~~~~~HM-METHOD~~~~~");
        // print_mg_str(hm->method);
        // print_flush("~~~~~HM-URI~~~~~");
        // print_mg_str(hm->uri);
        // print_flush("~~~~~HM-PROTO~~~~~");
        // print_mg_str(hm->proto);
        // print_flush("~~~~~~~~~~");

        //how to parse the content (the json stuff); ????
        print_flush("~~~~~~~~MESSAGE BODY, if any~~~~~~~~~~~~\n");
        print_mg_str(hm->body);
        print_flush("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

        if (is_equal(&hm->method, &s_post_method)) {
          printf("POST request\n");
          fflush(stdout);

          //Set up HTTP reply body to fill in from the if-statements

          //Parse the desired command by checking the key
          if(is_equal(&key, &key_add_node)){
            print_flush("KEY was add_node");
            //parse JSON 
            //return a vector of node id's: vector[0] and/or vector[1]
            //sanity check the size of vector
            //call wrapper function

            pair<int, vector<uint64_t> > result = parse_for_node_ids(hm->body, 1, "node_id");

            // struct json_token *tokens = parse_json2(hm->body.p, hm->body.len);
            // print_json_token(tokens);
            // struct json_token *token = find_json_token(tokens, "node_id");
            // print_json_token(token);
            // uint64_t node_id = token_to_uint64(token);
            // printf("Node_id found was: %u\n", node_id);
            // fflush(stdout);
          }
          else if(is_equal(&key, &key_add_edge)){
            print_flush("KEY was add_edge");
          }
          else if(is_equal(&key, &key_remove_node)){
            print_flush("KEY was remove_node");
          }
          else if(is_equal(&key, &key_remove_edge)){
            print_flush("KEY was remove_edge");
          }
          else if(is_equal(&key, &key_get_node)){
            print_flush("KEY was get_node");
          }
          else if(is_equal(&key, &key_get_edge)){
            print_flush("KEY was get_edge");
          }
          else if(is_equal(&key, &key_get_neighbors)){
            print_flush("KEY was get_neighbors");
          }
          else if(is_equal(&key, &key_shortest_path)){
            print_flush("KEY was shortest_path");
          }
          else{
            print_flush("KEY was not recognized!");
          }

        } 
        else {
          mg_printf(nc, "%s",
                    "HTTP/1.0 501 Not Implemented\r\n"
                    "Content-Length: 0\r\n\r\n");
          print_flush("Request was NOT a POST request");
        }
      } else {
        printf("Received unrecognized request...\n");
        fflush(stdout);
        mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      }
      break;
    default:
      break;
  }
}


int main(void) {

  //Initialize the graph in memory
  Graph graph;

  //Set up the server
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
