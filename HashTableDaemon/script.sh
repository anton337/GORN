#!/bin/bash

PWD=`pwd`
echo "$PWD"

max=4
for (( i=0 ; i < $max ; i++ ))
do 
    echo "$i"
	./HashTableDaemon "$PWD"/configs/node"$i".xml &
done

