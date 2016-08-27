#!/bin/bash

while true; do
    clear;
    ls batch_output;
    ls -s sorted_output;
    tail --bytes=1K sorted_output/comprehensive.out;
    sleep 1
done

