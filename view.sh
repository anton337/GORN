#!/bin/bash

while true; do
    clear;
    #ls queue_data;
    #ls queue_data_non_unique;
    ls batch_output;
    du -sh queue_data_non_unique;
    du -sh queue_data;
    du -sh sorted_output/comprehensive.out;
    du -sh sorted_output/comprehensive.tmp;
    tail --bytes=10K sorted_output/comprehensive.out;
    sleep 1
done

