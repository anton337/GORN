#!/bin/bash

source env/set_env.bash

${GORN_BIN}test_status_check
${GORN_BIN}test_system
${GORN_BIN}test_sort
${GORN_BIN}test_crawler
${GORN_BIN}test_serializer
${GORN_BIN}test_queue
${GORN_BIN}test_find


