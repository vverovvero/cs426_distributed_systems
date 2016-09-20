//api.cpp											Wendy Chen, 09/2016

//API of functions that server can call to manipulate the graph.
//Calls wrapper to manipulate graph, then returns result
//appropriate as HTTP reply and JSON

#include "api.h"
#include "graph.h"

#include <utility>
#include <map>
#include <set>

using std::pair;
using std::map;
using std::set;

//Return 1 when finished

int event_add_node(uint64_t node_id);
int event_add_edge(uint64_t node_a_id, uint64_t node_b_id);
int event_remove_node(uint64_t node_id);
int event_remove_edge(uint64_t node_a_id, uint64_t node_b_id);
int event_get_node(uint64_t node_id);
int event_get_edge(uint64_t node_a_id, uint64_t node_b_id);
int event_get_neighbors(uint64_t node_id);
int event_shortest_path(uint64_t node_a_id, uint64_t node_b_id);