#!/bin/bash

PID_dsd=`pidof DataSorterDaemon`
kill -9 $PID_dsd
PID_dq=`pidof DistributedQueue`
kill -9 $PID_dq
pkill HashTableDaemon

