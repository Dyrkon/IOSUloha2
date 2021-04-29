#!/bin/fish

export LC_NUMERIC=en_US.UTF-8

PROCESSES=$(pgrep proj2)

for PROC in $PROCESSES
do
    kill pgrep proj2
done