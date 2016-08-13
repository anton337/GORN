#!/bin/bash

source env/set_env.bash

cd DataGenerator; make; cd ..
cd ConfigGenerator; make; cd ..
cd HashTableDaemon; make; cd ..
cd ClientInterface; make; cd ..
cd DataSorterDaemon; make; cd ..
cd unit_tests; make; cd ..

