#!/bin/bash

# Initialize variables
data_file="response_times.txt"
rm -f "$data_file" # Remove the file if it already exists

#changing the working directory for getting the logs file
current_directory=$(pwd)
echo "Current directory is for analysis: $current_directory"
cd ./output/connection_logs

# Loop through the result files and calculate averages
for file in *.txt; do
	clients=$(echo "$file" | awk -F'_' '{print $3}')
    avg=$(awk '$0  { sum += $2 } END { print sum / NR }' "$file")
	echo "$clients $avg"
    echo "$clients $avg" >> "$data_file"
done

# Create a Gnuplot script
gnuplot_script="plot_response_times.gp"
echo "set term png" > "$gnuplot_script"
echo "set output 'response_times.png'" >> "$gnuplot_script"
echo "set title 'Average Response Time vs. Number of Clients'" >> "$gnuplot_script"
echo "set xlabel 'Number of Clients'" >> "$gnuplot_script"
echo "set ylabel 'Average Response Time (Seconds)'" >> "$gnuplot_script"
echo "set style data lines" >> "$gnuplot_script"
echo "plot 'response_times.txt' using 1:2 title 'Average Response Time'" >> "$gnuplot_script"

# Run Gnuplot with the script
gnuplot "$gnuplot_script"

# Remove the Gnuplot script
rm -f "$gnuplot_script"

# Initialize variables
data_file="throughput.txt"
rm -f "$data_file" # Remove the file if it already exists

# Loop through the result files and calculate averages
for file in *.txt; do
    clients=$(echo "$file" | awk -F'_' '{print $3}')
    avg=$(awk '{ sum += $3 } END { print sum / NR }' "$file")
    echo "$clients $avg" >> "$data_file"
done

# Create a Gnuplot script
gnuplot_script="plot_throughput.gp"
echo "set term png" > "$gnuplot_script"
echo "set output 'throughput.png'" >> "$gnuplot_script"
echo "set title 'Average Throughput vs. Number of Clients'" >> "$gnuplot_script"
echo "set xlabel 'Number of Clients'" >> "$gnuplot_script"
echo "set ylabel 'Average Throughput (NumOfResponses/sec)'" >> "$gnuplot_script"
echo "set style data lines" >> "$gnuplot_script"
echo "plot 'throughput.txt' using 1:2 title 'Average Throughput'" >> "$gnuplot_script"

# Run Gnuplot with the script
gnuplot "$gnuplot_script"

# Remove the Gnuplot script
rm -f "$gnuplot_script"

