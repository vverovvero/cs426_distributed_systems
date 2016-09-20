//api.h													Wendy Chen, 09/2016

//Contains #include, using, and functions for HTTP replies.
//Each function calls a wrapper from graph.h to manipulate the graph,
//then returns the appropriate HTTP reply/JSON.

#include "graph.h"

#include <utility>
#include <map>
#include <set>

using std::pair;
using std::map;
using std::set;

#ifdef API_H
#define API_H

int event_add_node(uint64_t node_id);
int event_add_edge(uint64_t node_a_id, uint64_t node_b_id);
int event_remove_node(uint64_t node_id);
int event_remove_edge(uint64_t node_a_id, uint64_t node_b_id);
int event_get_node(uint64_t node_id);
int event_get_edge(uint64_t node_a_id, uint64_t node_b_id);
int event_get_neighbors(uint64_t node_id);
int event_shortest_path(uint64_t node_a_id, uint64_t node_b_id);

#endif