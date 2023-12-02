#!/bin/bash

# Check if all required arguments are provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <port>"
    exit 1
fi

# Read command-line arguments
port="$1"

gcc -o gradingserver gradingserver.c
./gradingserver "$port"

echo "Server has completed."