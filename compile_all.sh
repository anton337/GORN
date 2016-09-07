#!/bin/bash

source env/set_env.bash

cd Crawler; make; cd ..
cd DistributedQueue; make; cd ..
cd StatusViewer; make; cd ..
cd unit_tests; make; cd ..
cd DataGenerator; make; cd ..
cd ConfigGenerator; make; cd ..
cd HashTableDaemon; make; cd ..
cd ClientInterface; make; cd ..
cd DataSorterDaemon; make; cd ..

