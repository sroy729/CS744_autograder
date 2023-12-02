#!/bin/bash

# # Set the range of M values to test
# M_values=(1 2 4 8 16 32 64)
# serverIP='10.129.131.243'
serverIP='127.0.0.0'
# port='5000'
program='p1.c'

# Check if all required arguments are provided
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <port> <num_clients> <num_iterations> <sleep_time>"
    exit 1
fi

# Read command-line arguments
port="$1"
num_clients="$2"
num_iterations="$3"
sleep_time="$4"

gcc -o loadGenerator loadGenerator.c 

# Start clients in the background
for ((i = 0; i < num_clients; i++)); do
    gnome-terminal -- bash -c "./loadGenerator "$serverIP" "$port" "$program" "$num_iterations" "$sleep_time" "$i" "$num_clients"; exit" &
done

echo "All clients started successfully."


# echo $M_values

# # Function to calculate throughput and average response time
# calculate_metrics() {
#   local client_id=$1
#   local output_file="client_${client_id}_output.txt"

#   # Simulate client activity and save output to the file
#   # Replace the following line with your actual client execution command
#   echo "Client $client_id output" > "$output_file"

#   # Calculate throughput for this client (you need to parse the actual output)
#   throughput=$(<"$output_file" wc -l)

#   # Calculate response time for this client (you need to parse the actual output)
#   response_time=10  # Replace with the actual response time

#   echo "Client $client_id: Throughput=$throughput, Response Time=$response_time" >> metrics.txt
# }

# # Loop through the different values of M
# for M in "${M_values[@]}"; do
#   # Start M clients in the background
#   for ((i = 1; i <= M; i++)); do
#     calculate_metrics "$i" &
#   done

#   # Wait for all clients to finish
#   wait

#   # Calculate overall throughput and average response time
#   total_throughput=0
#   total_response_time=0

# #   while IFS= read -r line; do
# #     if [[ $line =~ "Throughput=" ]]; then
# #       throughput=$(echo "$line" | awk -F'=' '{print $2}')
# #       total_throughput=$((total_throughput + throughput))
# #     elif [[ $line =~ "Response Time=" ]]; then
# #       response_time=$(echo "$line" | awk -F'=' '{print $2}')
# #       total_response_time=$((total_response_time + response_time))
# #     fi
# #   done < metrics.txt

#   average_response_time=$(echo "scale=2; $total_response_time / $M" | bc)

#   # Print and store results
#   echo "M=$M, Overall Throughput=$total_throughput, Average Response Time=$average_response_time"

#   # Clear the metrics file for the next iteration
#   > metrics.txt
# done

# # Optionally, you can create a plot using the collected data.
