#!/bin/bash

##Building simple graph 
#0: 1, 4
#1: 0
#2: 0, 3, 5
#3: 2, 5
#4: 0, 1
#5: 2, 3

#Need to test: add_node, add_edge, remove_node, remove_edge, get_node, get_edge, get_neighbors, shortest_path

echo "Building graph..."

#Adding nodes
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 0}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 1}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 2}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 3}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 4}' http://127.0.0.1:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 5}' http://127.0.0.1:8000/api/v1/add_node

#Adding edges
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 1}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 4}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 3}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 5}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 2}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 5}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 1}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 2}' http://127.0.0.1:8000/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 3}' http://127.0.0.1:8000/api/v1/add_edge

#Getting node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 0}' http://127.0.0.1:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 1}' http://127.0.0.1:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 2}' http://127.0.0.1:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 3}' http://127.0.0.1:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 4}' http://127.0.0.1:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 5}' http://127.0.0.1:8000/api/v1/get_node

#Getting edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 1}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 4}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 3}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 5}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 2}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 5}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 1}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 2}' http://127.0.0.1:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 3}' http://127.0.0.1:8000/api/v1/get_edge


#Get neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 0}' http://127.0.0.1:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 1}' http://127.0.0.1:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 2}' http://127.0.0.1:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 3}' http://127.0.0.1:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 4}' http://127.0.0.1:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 5}' http://127.0.0.1:8000/api/v1/get_neighbors


#Get shortest path
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 3}' http://127.0.0.1:8000/api/v1/shortest_path
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 5}' http://127.0.0.1:8000/api/v1/shortest_path
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 12}' http://127.0.0.1:8000/api/v1/shortest_path
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 1}' http://127.0.0.1:8000/api/v1/shortest_path


#Remove edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 1}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 0, "node_b_id": 4}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 3}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 5}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 2}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 5}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 0}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 1}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 2}' http://127.0.0.1:8000/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 3}' http://127.0.0.1:8000/api/v1/remove_edge


#Remove node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 0}' http://127.0.0.1:8000/api/v1/remove_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 1}' http://127.0.0.1:8000/api/v1/remove_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 2}' http://127.0.0.1:8000/api/v1/remove_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 3}' http://127.0.0.1:8000/api/v1/remove_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 4}' http://127.0.0.1:8000/api/v1/remove_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id": 5}' http://127.0.0.1:8000/api/v1/remove_node





