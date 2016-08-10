#!/bin/bash

source env/set_env.bash

${GORN_BIN}DataGenerator $1 $2;
more ${GORN_DATA}synthetic_test_data;

