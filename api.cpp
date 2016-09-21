//api.cpp											Wendy Chen, 09/2016

//API of functions that server can call to manipulate the graph.
//Calls wrapper to manipulate graph, then returns result
//appropriate as HTTP reply and JSON

#include "api.h"
#include "graph.h"
#include "mongoose.h"

#include <utility>
#include <map>
#include <set>

using std::pair;
using std::map;
using std::set;

//Return HTTP reply??

int event_add_node(Graph graph, struct mg_connection *nc, uint64_t node_id){
	//Call graph function
	int result = graph.add_node(node_id);

	//Send HTTP reply
	if(result == 1){
		//This has a payload
		mg_send_head(nc, 200, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
        char buf[1000];
        json_emit(buf, sizeof(buf), "{\n  s: i\n}\n", "node_id", node_id);
        mg_printf_http_chunk(nc, "%s", buf);
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished

	} 
	else {
		//This has no payload
		mg_send_head(nc, 204, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 204, "No Content\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}

	return 1;
}

int event_add_edge(Graph graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id){
	//Call graph function
	int result = graph.add_edge(node_a_id, node_b_id);

	//Send HTTP reply
	if(result == 1){
		//This has a payload
		mg_send_head(nc, 200, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
        char buf[1000];
        json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id);
        mg_printf_http_chunk(nc, "%s", buf);
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	else if(result == 0){
		//This has no payload
		mg_send_head(nc, 204, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 204, "No Content\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	else{
		mg_send_head(nc, 400, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "Bad Request\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	return 1;
}

int event_remove_node(Graph graph, struct mg_connection *nc, uint64_t node_id){
	//Call graph function
	int result = graph.remove_node(node_id);

	//Send HTTP reply
	if(result == 1){
		//This has a payload
		mg_send_head(nc, 200, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
        char buf[1000];
        json_emit(buf, sizeof(buf), "{\n  s: i\n}\n", "node_id", node_id);
        mg_printf_http_chunk(nc, "%s", buf);
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished

	} 
	else {
		//This has no payload
		mg_send_head(nc, 400, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "No Content\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	return 1;
}

int event_remove_edge(Graph graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id){
	//Call graph function
	int result = graph.remove_edge(node_a_id, node_b_id);

	//Send HTTP reply
	if(result == 1){
		//This has a payload
		mg_send_head(nc, 200, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
        char buf[1000];
        json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id);
        mg_printf_http_chunk(nc, "%s", buf);
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	else if(result == 0){
		//This has no payload
		mg_send_head(nc, 400, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "No Content\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}

	return 1;
}

int event_get_node(Graph graph, struct mg_connection *nc, uint64_t node_id){
	//Call graph function
	int result = graph.get_node(node_id);

	//Send HTTP reply
	mg_send_head(nc, 200, -1, NULL);
    mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
    char buf[1000];
    json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i\n}\n", "node_id", node_id, "in_graph", result);
    mg_printf_http_chunk(nc, "%s", buf);
    mg_send_http_chunk(nc, "", 0); // Tell the client we're finished

	return 1;
}

int event_get_edge(Graph graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id){
	//Call graph function
	int result = graph.get_edge(node_a_id, node_b_id);

	//Send HTTP reply
	if(result != 2){
		mg_send_head(nc, 200, -1, NULL);
	    mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
	    char buf[1000];
	    json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id, "in_graph", result);
	    mg_printf_http_chunk(nc, "%s", buf);
	    mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	else{
		mg_send_head(nc, 400, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "No Content\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}

	return 1;
}

int event_get_neighbors(Graph graph, struct mg_connection *nc, uint64_t node_id){
	//Call graph function
	pair<int, set<uint64_t> > result = graph.get_neighbors(node_id);

	//Send HTTP reply
	if(result.first == 1){
		set<uint64_t> neighbors = result.second;

		//Send list of neighbors
		mg_send_head(nc, 200, -1, NULL);
	    mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
	    char buf[1000];
	    char buf1[1000];
	    char buf2[1000];
	    json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: [", "node_id", node_id, "neighbors");
	    for(set<uint64_t>::iterator i = neighbors.begin(); i != neighbors.end(); i++){
			uint64_t neighbor = *i;
			json_emit(buf1, sizeof(buf1), "s,", neighbor);
		}
		json_emit(buf2, sizeof(buf2), "]\n}\n");
	    mg_printf_http_chunk(nc, "%s", buf);
	    mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	else{
		mg_send_head(nc, 400, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "No Content\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}

	return 1;
}

int event_shortest_path(Graph graph, struct mg_connection *nc, uint64_t node_a_id, uint64_t node_b_id){
	//Call graph function
	pair<int, uint64_t> result = graph.shortest_path(node_a_id, node_b_id);

	//Send HTTP reply
	if(result.first == 1){
		mg_send_head(nc, 200, -1, NULL);
	    mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 200, "OK\n");
	    char buf[1000];
	    json_emit(buf, sizeof(buf), "{\n  s: i,\n  s: i,\n  s: i\n}\n", "node_a_id", node_a_id, "node_b_id", node_b_id, "in_graph", result.second);
	    mg_printf_http_chunk(nc, "%s", buf);
	    mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	else if(result.first == 0){
		mg_send_head(nc, 204, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 204, "No Content\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}
	else{
		mg_send_head(nc, 400, -1, NULL);
        mg_printf_http_chunk(nc, "%s %d %s", "HTTP/1.1 ", 400, "Bad Request\n");
        mg_send_http_chunk(nc, "", 0); // Tell the client we're finished
	}

	return 1;
}








