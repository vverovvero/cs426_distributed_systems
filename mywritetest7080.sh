#!/bin/bash

echo "Beginning write test script (only send to head 7080!)"

#We will build this graph:
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

#Add nodes
for i in {1..11}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.145.208:7080/api/v1/add_node
done

#duplication add node test
for i in {3..6}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.145.208:7080/api/v1/add_node
done

#Add edges (with dups)
#node 1: 2, 3, 4
#node 2: 1, 3
#node 3: 2, 1
#node 4: 1, 6, 7
#node 5: 1, 8
#node 6: 4
#node 7: 4
#node 8: 5, 9
#node 9: 8, 10, 11
#node 10: 9
#node 11: 9
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 2}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 3}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 4}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 3}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 2}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 6}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 7}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 8}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 6, "node_b_id": 4}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 7, "node_b_id": 4}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 8, "node_b_id": 5}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 8, "node_b_id": 9}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 9, "node_b_id": 8}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 9, "node_b_id": 10}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 9, "node_b_id": 11}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 10, "node_b_id": 9}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 11, "node_b_id": 9}' http://104.154.145.208:7080/api/v1/add_edge


#Add bad edges (ie, nonexistent ones, edge to itself)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 21}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge

#remove edges
for i in {1..10} 
do
	for j in {2..11}
	do
		curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":'$i', "node_b_id":'$j'}' http://104.154.145.208:7080/api/v1/remove_edge
	done
done

#re-remove edges bad (dups, self, nonexistent)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 32}' http://104.154.145.208:7080/api/v1/add_edge


#Remove nodes
for i in {1..11}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.145.208:7080/api/v1/remove_node
done

#re-remove nodes
for i in {1..4}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.145.208:7080/api/v1/remove_node
done

#build final graph (add nodes, add edges)
for i in {1..11}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://104.154.145.208:7080/api/v1/add_node
done

curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 2}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 3}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 1, "node_b_id": 4}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 2, "node_b_id": 3}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 3, "node_b_id": 2}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 6}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 4, "node_b_id": 7}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 1}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 5, "node_b_id": 8}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 6, "node_b_id": 4}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 7, "node_b_id": 4}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 8, "node_b_id": 5}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 8, "node_b_id": 9}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 9, "node_b_id": 8}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 9, "node_b_id": 10}' http://104.154.145.208:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id": 10, "node_b_id": 9}' http://104.154.145.208:7080/api/v1/add_edge


#curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://104.154.145.208:7080/api/v1/checkpoint