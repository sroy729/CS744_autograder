#!/bin/bash
: '
Description	: For simulating simulataneous client connection 
Usage		:
Author		: Preeti, Shubham
Comment		: To connect to a specfic IP change the serverIP variable also change the port accordingly
'

# # Set the range of M values to test
# M_values=(1 2 4 8 16 32 64)
serverIP='127.0.0.0'

# Check if all required arguments are provided
if [ "$#" -ne 5 ]; then
    echo "Usage: $0 <port> <num_clients> <num_iterations> <sleep_time> <grading_file>"
    exit 1
fi

# Read command-line arguments
port="$1"
num_clients="$2"
num_iterations="$3"
sleep_time="$4"
program="$5"

cd "${PWD%/*}"
make

# Start clients in the background
for ((i = 1; i <= num_clients; i++)); do
    gnome-terminal -- bash -c "./bin/client "$serverIP:$port" "$program" "$num_iterations" "$sleep_time" "$i" "$num_clients"; exit" &
done

echo "All clients started successfully."

