//api.h													Wendy Chen, 09/2016

//Contains #include, using, and functions for HTTP replies.
//Each function calls a wrapper from graph.h to manipulate the graph,
//then returns the appropriate HTTP reply/JSON.

#include "graph.h"
#include "stdint.h"

#include <utility>
#include <map>
#include <set>
#include <string>
#include <sstream>

using std::pair;
using std::map;
using std::set;
using std::string;

#ifndef API_H
#define API_H

void emit_json_start(struct mg_connection *nc, const int code);
void emit_json_header(struct mg_connection *nc, const int code, const char *message);
void emit_json_body(struct mg_connection *nc, const char *buf, const int size);
void emit_json_end(struct mg_connection *nc);

void event_add_node(Graph *graph, struct mg_connection *nc, uint64_t node_id);
void event_add_edge(Graph *graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id);
void event_remove_node(Graph *graph, struct mg_connection *nc, uint64_t node_id);
void event_remove_edge(Graph *graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id);
void event_get_node(Graph *graph, struct mg_connection *nc, uint64_t node_id);
void event_get_edge(Graph *graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id);
void event_get_neighbors(Graph *graph, struct mg_connection *nc, uint64_t node_id);
void event_shortest_path(Graph *graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id);


#endif