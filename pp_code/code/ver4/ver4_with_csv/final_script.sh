#!/bin/bash

# Initialize variables
data_file="response_times.txt"
rm -f "$data_file" # Remove the file if it already exists

# Loop through the result files and calculate averages
for file in *_AvgResponseTime.txt; do
    clients=$(echo "$file" | awk -F'_' '{print $1}')
    avg=$(awk '{ sum += $1 } END { print sum / NR }' "$file")
    echo "$clients $avg" >> "$data_file"
done

# Create a Gnuplot script
gnuplot_script="plot_response_times.gp"
echo "set term png" > "$gnuplot_script"
echo "set output 'response_times.png'" >> "$gnuplot_script"
echo "set title 'Average Response Time vs. Number of Clients'" >> "$gnuplot_script"
echo "set xlabel 'Number of Clients'" >> "$gnuplot_script"
echo "set ylabel 'Average Response Time (nanoSeconds)'" >> "$gnuplot_script"
echo "set style data lines" >> "$gnuplot_script"
echo "plot 'response_times.txt' using 1:2 title 'Average Response Time'" >> "$gnuplot_script"

# Run Gnuplot with the script
gnuplot "$gnuplot_script"

# Remove the Gnuplot script
rm "$gnuplot_script"

# Initialize variables
data_file="throughput.txt"
rm -f "$data_file" # Remove the file if it already exists

# Loop through the result files and calculate averages
for file in *_AvgThroughput.txt; do
    clients=$(echo "$file" | awk -F'_' '{print $1}')
    avg=$(awk '{ sum += $1 } END { print sum / NR }' "$file")
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
rm "$gnuplot_script"


# Initialize variables
data_file="numTimeout.txt"
rm -f "$data_file" # Remove the file if it already exists

# Loop through the result files and calculate averages
for file in *_numTimeouts.txt; do
    clients=$(echo "$file" | awk -F'_' '{print $1}')
    avg=$(awk '{ sum += $1 } END { print sum / NR }' "$file")
    echo "$clients $avg" >> "$data_file"
done

# Create a Gnuplot script
gnuplot_script="plot_numTimeout.gp"
echo "set term png" > "$gnuplot_script"
echo "set output 'numTimeout.png'" >> "$gnuplot_script"
echo "set title 'Number of Timeouts vs. Number of Clients'" >> "$gnuplot_script"
echo "set xlabel 'Number of Clients'" >> "$gnuplot_script"
echo "set ylabel 'Number of Timeouts'" >> "$gnuplot_script"
echo "set style data lines" >> "$gnuplot_script"
echo "plot 'numTimeout.txt' using 1:2 title 'Timeouts Rate'" >> "$gnuplot_script"

# Run Gnuplot with the script
gnuplot "$gnuplot_script"

# Remove the Gnuplot script
rm "$gnuplot_script"


# Initialize variables
data_file="reqSentRate.txt"
rm -f "$data_file" # Remove the file if it already exists

# Loop through the result files and calculate averages
for file in *_reqSentRate.txt; do
    clients=$(echo "$file" | awk -F'_' '{print $1}')
    avg=$(awk '{ sum += $1 } END { print sum / NR }' "$file")
    echo "$clients $avg" >> "$data_file"
done

# Create a Gnuplot script
gnuplot_script="plot_reqSentRate.gp"
echo "set term png" > "$gnuplot_script"
echo "set output 'reqSentRate.png'" >> "$gnuplot_script"
echo "set title 'Request Sent Rate vs. Number of Clients'" >> "$gnuplot_script"
echo "set xlabel 'Number of Clients'" >> "$gnuplot_script"
echo "set ylabel 'Request Sent Rate'" >> "$gnuplot_script"
echo "set style data lines" >> "$gnuplot_script"
echo "plot 'reqSentRate.txt' using 1:2 title 'Request Sent Rate'" >> "$gnuplot_script"

# Run Gnuplot with the script
gnuplot "$gnuplot_script"

# Remove the Gnuplot script
rm "$gnuplot_script"