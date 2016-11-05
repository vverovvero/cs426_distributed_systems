#!/bin/bash

echo "Beginning test script"

for i in {1..500}
do
	curl -H "Content-Type: application/json" -X POST -d '{"node_id":'$i'}' http://127.0.0.1:8000/api/v1/add_node
done