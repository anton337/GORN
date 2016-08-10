#!/bin/bash

source env/set_env.bash

cd ${GORN_CONFIG} ; rm -f *.xml ; ${GORN_BIN}GenerateClusterConfigs $1 $2 ; cd ..
more ${GORN_CONFIG}*.xml;

