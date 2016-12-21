#!/bin/bash

echo "Beginning test script"

##Assume we have
#./cs426_graph_server 7080 -p 1 -l 104.198.165.44:8080 104.154.198.82:8090 104.154.145.208:9000
#./cs426_graph_server 7090 -p 2 -l 104.198.165.44:8080 104.154.198.82:8090 104.154.145.208:9000
#./cs426_graph_server 8000 -p 3 -l 104.198.165.44:8080 104.154.198.82:8090 104.154.145.208:9000

#also test bad get_edges

############ ADD NODES ##############
#Partition 1: Add nodes 0, 3, 6
curl -H "Content-Type: application/json" -X POST -d '{"node_id":0}' http://104.198.165.44:7080/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":3}' http://104.198.165.44:7080/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":6}' http://104.198.165.44:7080/api/v1/add_node

#Partition 2: Add nodes 1, 4, 7
curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://104.154.198.82:7090/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":4}' http://104.154.198.82:7090/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":7}' http://104.154.198.82:7090/api/v1/add_node

#Partition 3: Add nodes 2, 5, 8
curl -H "Content-Type: application/json" -X POST -d '{"node_id":2}' http://104.154.145.208:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":5}' http://104.154.145.208:8000/api/v1/add_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":8}' http://104.154.145.208:8000/api/v1/add_node

############ GET NODES ##############
#Partition 1: Get nodes 0, 3, 6
curl -H "Content-Type: application/json" -X POST -d '{"node_id":0}' http://104.198.165.44:7080/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":3}' http://104.198.165.44:7080/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":6}' http://104.198.165.44:7080/api/v1/get_node

#Partition 2: Get nodes 1, 4, 7
curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://104.154.198.82:7090/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":4}' http://104.154.198.82:7090/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":7}' http://104.154.198.82:7090/api/v1/get_node

#Partition 3: Get nodes 2, 5, 8
curl -H "Content-Type: application/json" -X POST -d '{"node_id":2}' http://104.154.145.208:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":5}' http://104.154.145.208:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":8}' http://104.154.145.208:8000/api/v1/get_node

############ ADD EDGES ##############
#Partition 1: Add edges (0,1), (2,3), (3,4), (5,6), (6,7)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":0, "node_b_id":1}' http://104.198.165.44:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":3}' http://104.198.165.44:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":3, "node_b_id":4}' http://104.198.165.44:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":5, "node_b_id":6}' http://104.198.165.44:7080/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":6, "node_b_id":7}' http://104.198.165.44:7080/api/v1/add_edge

#Partition 2: Add edges (1,2), (4,5), (7,8)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://104.154.198.82:7090/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":4, "node_b_id":5}' http://104.154.198.82:7090/api/v1/add_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":7, "node_b_id":8}' http://104.154.198.82:7090/api/v1/add_edge



############ GET EDGES ##############
#Partition 1: Get edges (0,1), (2,3), (3,4), (5,6), (6,7)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":0, "node_b_id":1}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":3}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":3, "node_b_id":4}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":5, "node_b_id":6}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":6, "node_b_id":7}' http://104.198.165.44:7080/api/v1/get_edge


#Partition 2: Get edges (1,2), (4,5), (7,8)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":4, "node_b_id":5}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":7, "node_b_id":8}' http://104.154.198.82:7090/api/v1/get_edge


#Partition 3: Get edges (1,2), (2,3), (4,5), (5,6), (7,8)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":3}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":4, "node_b_id":5}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":5, "node_b_id":6}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":7, "node_b_id":8}' http://104.154.145.208:8000/api/v1/get_edge


############ GET NEIGHBORS ##############
#Partition 1: Get neighbors of nodes 0, 3, 6
curl -H "Content-Type: application/json" -X POST -d '{"node_id":0}' http://104.198.165.44:7080/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":3}' http://104.198.165.44:7080/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":6}' http://104.198.165.44:7080/api/v1/get_neighbors

#Partition 2: Get neighbors of nodes 1, 4, 7
curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://104.154.198.82:7090/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":4}' http://104.154.198.82:7090/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":7}' http://104.154.198.82:7090/api/v1/get_neighbors

#Partition 3: Get neighbors of nodes 2, 5, 8
curl -H "Content-Type: application/json" -X POST -d '{"node_id":2}' http://104.154.145.208:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":5}' http://104.154.145.208:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":8}' http://104.154.145.208:8000/api/v1/get_neighbors

############ REMOVE EDGES ##############
#Partition 1: Remove edges (0,1), (2,3), (3,4), (5,6), (6,7)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":0, "node_b_id":1}' http://104.198.165.44:7080/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":3}' http://104.198.165.44:7080/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":3, "node_b_id":4}' http://104.198.165.44:7080/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":5, "node_b_id":6}' http://104.198.165.44:7080/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":6, "node_b_id":7}' http://104.198.165.44:7080/api/v1/remove_edge

#Partition 2: Remove edges (1,2), (4,5), (7,8)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://104.154.198.82:7090/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":4, "node_b_id":5}' http://104.154.198.82:7090/api/v1/remove_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":7, "node_b_id":8}' http://104.154.198.82:7090/api/v1/remove_edge



############ GET EDGES  AGAIN##############
#Partition 1: Get edges (0,1), (2,3), (3,4), (5,6), (6,7)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":0, "node_b_id":1}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":3}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":3, "node_b_id":4}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":5, "node_b_id":6}' http://104.198.165.44:7080/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":6, "node_b_id":7}' http://104.198.165.44:7080/api/v1/get_edge


#Partition 2: Get edges (1,2), (4,5), (7,8)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":4, "node_b_id":5}' http://104.154.198.82:7090/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":7, "node_b_id":8}' http://104.154.198.82:7090/api/v1/get_edge


#Partition 3: Get edges (1,2), (2,3), (4,5), (5,6), (7,8)
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":1, "node_b_id":2}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":2, "node_b_id":3}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":4, "node_b_id":5}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":5, "node_b_id":6}' http://104.154.145.208:8000/api/v1/get_edge
curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":7, "node_b_id":8}' http://104.154.145.208:8000/api/v1/get_edge


############ GET NEIGHBORS  AGAIN ##############
#Partition 1: Get neighbors of nodes 0, 3, 6
curl -H "Content-Type: application/json" -X POST -d '{"node_id":0}' http://104.198.165.44:7080/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":3}' http://104.198.165.44:7080/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":6}' http://104.198.165.44:7080/api/v1/get_neighbors

#Partition 2: Get neighbors of nodes 1, 4, 7
curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://104.154.198.82:7090/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":4}' http://104.154.198.82:7090/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":7}' http://104.154.198.82:7090/api/v1/get_neighbors

#Partition 3: Get neighbors of nodes 2, 5, 8
curl -H "Content-Type: application/json" -X POST -d '{"node_id":2}' http://104.154.145.208:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":5}' http://104.154.145.208:8000/api/v1/get_neighbors
curl -H "Content-Type: application/json" -X POST -d '{"node_id":8}' http://104.154.145.208:8000/api/v1/get_neighbors


############ GET NODES AGAIN ##############
#Partition 1: Get nodes 0, 3, 6
curl -H "Content-Type: application/json" -X POST -d '{"node_id":0}' http://104.198.165.44:7080/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":3}' http://104.198.165.44:7080/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":6}' http://104.198.165.44:7080/api/v1/get_node

#Partition 2: Get nodes 1, 4, 7
curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://104.154.198.82:7090/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":4}' http://104.154.198.82:7090/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":7}' http://104.154.198.82:7090/api/v1/get_node

#Partition 3: Get nodes 2, 5, 8
curl -H "Content-Type: application/json" -X POST -d '{"node_id":2}' http://104.154.145.208:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":5}' http://104.154.145.208:8000/api/v1/get_node
curl -H "Content-Type: application/json" -X POST -d '{"node_id":8}' http://104.154.145.208:8000/api/v1/get_node




