#!/bin/bash

source env/set_env.bash

for i in $(find ${GORN_CONFIG} -name "*.xml")
do 
    echo "$i"
	${GORN_BIN}HashTableDaemon "$i" &
done

${GORN_BIN}DataSorterDaemon &

