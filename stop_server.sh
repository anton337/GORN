#!/bin/bash

PID=`pidof DataSorterDaemon`
kill -9 $PID
pkill HashTableDaemon

