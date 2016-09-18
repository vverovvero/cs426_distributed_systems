// Copyright (c) 2015 Cesanta Software Limited
// All rights reserved

#include "mongoose.h"
#include "api.h"
#include "graph.h"

static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    printf("Received a request");
    fflush(stdout);
    mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);
  }
}

int main(void) {

/////////TEMPORARY TEST CODE////////////////////
  Graph graph;
  graph.add_node(1);
  graph.add_node(2);
  graph.add_node(3);
  graph.add_node(4);
  graph.add_edge(1,2);
  graph.add_edge(5,5);
  graph.add_edge(4,6);
  graph.add_edge(1,2);
  graph.get_neighbors(1);
  graph.remove_node(7);
  graph.remove_node(4);
  graph.remove_edge(1,2);
  graph.remove_edge(1,3);

///////////SERVER CODE//////////////////////////
  // struct mg_mgr mgr;
  // struct mg_connection *nc;

  // mg_mgr_init(&mgr, NULL);
  // nc = mg_bind(&mgr, s_http_port, ev_handler);

  // // Set up HTTP server parameters
  // mg_set_protocol_http_websocket(nc);
  // s_http_server_opts.document_root = ".";      // Serve current directory
  // s_http_server_opts.dav_document_root = ".";  // Allow access via WebDav
  // s_http_server_opts.enable_directory_listing = "yes";

  // printf("Starting web server on port %s\n", s_http_port);
  // for (;;) {
  //   mg_mgr_poll(&mgr, 1000);
  //   printf("Server is polling...");
  //   fflush(stdout);
  // }
  // mg_mgr_free(&mgr);

  // return 0;
}
