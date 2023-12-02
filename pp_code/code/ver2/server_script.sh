#!/bin/bash

# Check if all required arguments are provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <port>"
    exit 1
fi

# Read command-line arguments
port="$1"

vmstat -n 5 > cpu_stat.txt &
ps -eo pid,comm | grep vmstat | awk '{print $1}' > vmstat_pid.txt

gcc -o multiThreadServer multiThreadServer.c -lpthread
./multiThreadServer "$port"

echo "Server has completed."