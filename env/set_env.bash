#!/bin/bash

ROOT_PATH=$( pwd -P )

export GORN_ROOT="$ROOT_PATH"
export GORN_BIN="$ROOT_PATH/bin/"
export GORN_DATA="$ROOT_PATH/data/"
export GORN_UTILS="$ROOT_PATH/utils/"
export GORN_CONFIG="$ROOT_PATH/configs/"

export GORN_CXX="g++ -std=c++11"

#export GORN_CXX_FLAGS="-O3 -g0 -Wall"
export GORN_CXX_FLAGS="-O0 -g3 -Wall"

echo "GORN_ROOT=$GORN_ROOT"
echo "GORN_BIN=$GORN_BIN"
echo "GORN_DATA=$GORN_DATA"
echo "GORN_UTILS=$GORN_UTILS"
echo "GORN_CONFIG=$GORN_CONFIG"

