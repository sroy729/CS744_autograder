#!/bin/bash

# Read the PID from the file
VMSTAT_PID=$(<vmstat_pid.txt)

# Kill the background process
kill $VMSTAT_PID

# Initialize variables
#!/bin/bash

# Input file
input_file="cpu_stat.txt"
out_file="avgCpuUtil.txt"

# Column number (change this to the column you want to average)
column_number=15

# Initialize variables
sum=0
count=0

# Check if the file exists
if [ ! -f "$input_file" ]; then
    echo "Input file does not exist."
    exit 1
fi

# Use a while loop to read each line
tail -n +3 "$input_file" > "cpu.txt"
input_file="cpu.txt"
while read -r line; do
    # Use awk to extract the value from the specified column
    value=$(echo "$line" | awk -v col="$column_number" '{print 100-$col}')
    # echo "$value" >> cpu_util.txt

    # Check if the value is a number (non-empty)
    if [ -n "$value" ]; then
        sum=$((sum + value))
        count=$((count + 1))
    fi
done < "$input_file"

# Calculate the average
if [ "$count" -gt 0 ]; then
    average=$((sum / count))
    echo "$average" >> "$out_file"
    echo "Average of column $column_number: $average"
else
    echo "No valid values found in column $column_number."
fi

rm -f "cpu_util.txt" # Remove the file if it already exists
rm -f "cpu.txt" # Remove the file if it already exists
rm -f "vmstat_pid.txt" # Remove the file if it already exists