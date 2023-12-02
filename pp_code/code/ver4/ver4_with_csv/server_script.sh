#!/bin/bash

# Check if all required arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <port> <numThreads>"
    exit 1
fi

# Read command-line arguments
port="$1"
numThreads="$2"

vmstat -n 5 > cpu_stat.txt &
ps -eo pid,comm | grep vmstat | awk '{print $1}' > vmstat_pid.txt

gcc -o multiThreadServer multiThreadServer.c -lpthread
./multiThreadServer "$port" "$numThreads"

echo "Server has completed."