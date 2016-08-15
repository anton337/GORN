#!/bin/bash

source env/set_env.bash

${GORN_BIN}test_system
${GORN_BIN}test_sort
${GORN_BIN}test_crawler


