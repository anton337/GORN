#!/bin/bash

source env/set_env.bash

cd DataGenerator; make; cd ..
cd ConfigGenerator; make; cd ..
cd HashTableDaemon; make; cd ..
cd ClientInterface; make; cd ..

