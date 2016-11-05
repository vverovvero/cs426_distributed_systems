#!/bin/bash

echo "Beginning test script"

#Add nodes
for i in {1..500}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://127.0.0.1:8000/api/v1/add_node
done

#Add edges
for i in {1..200}
do
	j = i+1
	curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":'$i', "node_b_id":'$j'}' http://127.0.0.1:8000/api/v1/add_edge
done


#Remove nodes
for i in {100..200}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://127.0.0.1:8000/api/v1/remove_node
done

#remove edges
for i in {1..10}
do
	j = i+1
	curl -H "Content-Type: application/json" -X POST -d '{"node_a_id":'$i', "node_b_id":'$j'}' http://127.0.0.1:8000/api/v1/remove_edge
done

#curl -H "Content-Type: application/json" -X POST -d '{"node_id":1}' http://127.0.0.1:8000/api/v1/checkpoint