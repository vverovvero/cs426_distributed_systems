//api.cpp											Wendy Chen, 10/2016

//API of functions that server can call to manipulate the graph.
//Calls wrapper to manipulate graph, then returns result
//appropriate as HTTP reply and JSON

#include "api.h"
#include "graph.h"
#include "mongoose.h"
#include "stdint.h"

#include <utility>
#include <map>
#include <set>
// #include <string>
// #include <sstream>

using std::pair;
using std::map;
using std::set;
// using std::string;

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

//send before sending anything else
void emit_json_start(struct mg_connection *nc, const int code){
	mg_send_head(nc, code, -1, NULL); //-1 indicates chunked encoding	
}

//emits status line and message
void emit_json_header(struct mg_connection *nc, const int code, const char *message){
	//This has no payload
    mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", code, message);
}

//call after using json_emit (b/c size and buffer).  emits body
void emit_json_body(struct mg_connection *nc, const char *buf, const int size){
    mg_printf_http_chunk(nc, "%s %d %s", "Content-Length: ", size, "\n");
    mg_printf_http_chunk(nc, "%s", "Content-Type: application/json\n\n");
    mg_printf_http_chunk(nc, "%s", buf);
}

//send after sending everything
void emit_json_end(struct mg_connection *nc){
	mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
}

//////////////////////////////////////////////////////////////////////////////////
void event_add_node(Graph *graph, struct mg_connection *nc, uint64_t node_id){
	//Call graph function
	int result = (*graph).add_node(node_id);

	//Send HTTP reply
	if(result == 1){
		emit_json_start(nc, 200);
		emit_json_header(nc, 200, "OK\n");
        char buf[1000];
        int size = json_emit(buf, sizeof(buf), "{\n  s: i\n}\n", "node_id", node_id);
        emit_json_body(nc, buf, size);
        emit_json_end(nc);
	} 
	else if(result == 0){
		emit_json_start(nc, 204);
		emit_json_header(nc, 204, "No Content\n");
		emit_json_end(nc);
	}
	//2, Bad Request due to partition error
	else{
		emit_json_start(nc, 400);
		emit_json_header(nc, 400, "Bad Request\n");
		emit_json_end(nc);
	}
}

//Require RCP modification 
void event_add_edge(Graph *graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a){
	//Call graph function
	int result = (*graph).add_edge(node_a_id, node_b_id, node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);

	//Send HTTP reply
	if(result == 1){
		emit_json_start(nc, 200);
		emit_json_header(nc, 200, "OK\n");
        char buf[1000];
        int size = json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id);
        emit_json_body(nc, buf, size);
        emit_json_end(nc);
	}
	else if(result == 0){
		emit_json_start(nc, 204);
		emit_json_header(nc, 204, "No Content\n");
		emit_json_end(nc);
	}
	else{
		emit_json_start(nc, 400);
		emit_json_header(nc, 400, "Bad Request\n");
		emit_json_end(nc);
	}
}


//REQUIRE RPC MODIFICATION
void event_remove_edge(Graph *graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id, uint64_t node_a_exists, uint64_t node_b_exists, uint64_t node_a_has_b, uint64_t node_b_has_a){	
	//Call graph function
	int result = (*graph).remove_edge(node_a_id, node_b_id, node_a_exists, node_b_exists, node_a_has_b, node_b_has_a);

	//Send HTTP reply
	if(result == 1){
		emit_json_start(nc, 200);
		emit_json_header(nc, 200, "OK\n");
        char buf[1000];
        int size = json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id);
       	emit_json_body(nc, buf, size);
       	emit_json_end(nc);
	}
	else if(result == 0){
		emit_json_start(nc, 400);
		emit_json_header(nc, 400, "No Content\n");
		emit_json_end(nc);
	}
}

void event_get_node(Graph *graph, struct mg_connection *nc, uint64_t node_id){
	//Call graph function
	int result = (*graph).get_node(node_id);

	//Send HTTP reply
	if(result != 2){
		emit_json_start(nc, 200);
		emit_json_header(nc, 200, "OK\n");
	    int size;
	    char buf[1000];
	    if(result == 1){
	    	size = json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i\n}\n", "node_id", node_id, "in_graph", (long) 1);
	    }
	    else{
	    	size = json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i\n}\n", "node_id", node_id, "in_graph", (long) 0);
	    }
	    emit_json_body(nc, buf, size);
	    emit_json_end(nc);
	}
	else{
		emit_json_start(nc, 400);
		emit_json_header(nc, 400, "Bad Request\n");
		emit_json_end(nc);
	}

}

void event_get_edge(Graph *graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id){
	//Call graph function
	int result = (*graph).get_edge(node_a_id, node_b_id);
	//Send HTTP reply
	if(result != 2){
		emit_json_start(nc, 200);
		emit_json_header(nc, 200, "OK\n");
	    int size;
	    char buf[1000];
	    if(result == 1){
	    	size = json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id, "in_graph", (long) 1);
	    }
	    else{
	    	size = json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id, "in_graph", (long) 0);	
	    }
	    emit_json_body(nc, buf, size);
	    emit_json_end(nc);
	}
	else{
		emit_json_start(nc, 400);
		emit_json_header(nc, 400, "No Content\n");
		emit_json_end(nc);
	}
}

//version of get_neighbors without string
void event_get_neighbors(Graph *graph, struct mg_connection *nc, uint64_t node_id){
	//Call graph function
	pair<int, set<uint64_t> > result = (*graph).get_neighbors(node_id);

	//Send HTTP reply
	if(result.first == 1){
		set<uint64_t> neighbors = result.second;
		char c_neighbor_list[1000] = "";

		if(!neighbors.empty()){
			set<uint64_t>::iterator j; //peek_ahead
	   		for(set<uint64_t>::iterator i = neighbors.begin(); i != neighbors.end(); i++){
				uint64_t neighbor = *i;
				char c_neighbor[50];
				set<uint64_t>::iterator j = i;
				j++;
				if(j != neighbors.end()){
					sprintf(c_neighbor, "%lu, ", neighbor);
				}
				else{
					sprintf(c_neighbor, "%lu", neighbor);
				}
				strcat(c_neighbor_list, c_neighbor);
			}
		}

		//Send list of neighbors
		emit_json_start(nc, 200);
		emit_json_header(nc, 200, "OK\n");
	    char buf[1000];
	    int size = json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: [S]\n}\n", "node_id", node_id, "neighbors", c_neighbor_list);
	    emit_json_body(nc, buf, size);
	    emit_json_end(nc);
	}
	else{
		emit_json_start(nc, 400);
		emit_json_header(nc, 400, "Bad Request\n");
		emit_json_end(nc);
	}
}

