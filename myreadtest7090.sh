#!/bin/bash

echo "Beginning read test script 7090"

#We will read this graph:
#Add edges
#node 1: 2, 3, 4
#node 2: 1, 3
#node 3: 2, 1
#node 4: 1, 6, 7
#node 5: 1, 8
#node 6: 4
#node 7: 4
#node 8: 5, 9
#node 9: 8, 10
#node 10: 9
#node 11: none

#Get good nodes
for i in {1..11}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.198.82:7090/api/v1/get_node
done

#Get bad nodes (nonexistent)
for i in {12..13}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.198.82:7090/api/v1/get_node
done

#Get edges (some)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 2}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 3}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 7, "node_b_id": 4}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 9, "node_b_id": 10}' http://104.154.198.82:7090/api/v1/get_edge


#Get bad edges (ie, nonexistent ones, edge to itself)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 1}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 21}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 12, "node_b_id": 13}' http://104.154.198.82:7090/api/v1/get_edge


#get_neighbors
for i in {1..11}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.198.82:7090/api/v1/get_neighbors
done

#get_bad neighbors
for i in {12..13}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.198.82:7090/api/v1/get_neighbors
done

#Shortest path good
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 10}' http://104.154.198.82:7090/api/v1/shortest_path
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 6}' http://104.154.198.82:7090/api/v1/shortest_path

#shortst path bad (self, nonexistent)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 1}' http://104.154.198.82:7090/api/v1/shortest_path
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 21}' http://104.154.198.82:7090/api/v1/shortest_path
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 12, "node_b_id": 13}' http://104.154.198.82:7090/api/v1/shortest_path

