#!/bin/bash

# # Set the range of M values to test
# M_values=(1 2 4 8 16 32 64)
# serverIP='10.129.131.243'
serverIP='10.130.171.180'
# port='5000'
program='p1.c'

# Check if all required arguments are provided
if [ "$#" -ne 5 ]; then
    echo "Usage: $0 <port> <num_clients> <num_iterations> <sleep_time> <timeout sec>"
    exit 1
fi

# Read command-line arguments
port="$1"
num_clients="$2"
num_iterations="$3"
sleep_time="$4"
tos="$4"
# a=1

gcc -o loadGenerator loadGenerator.c -lpthread

# Start clients in the background
for ((i = 0; i < num_clients; i++)); do
    gnome-terminal -- bash -c "./loadGenerator "$serverIP" "$port" "$program" "$num_iterations" "$sleep_time" "$i" "$num_clients" "$tos"; exit" &
    # sleep_time=`expr $sleep_time + $a`
done

echo "All clients started successfully."

