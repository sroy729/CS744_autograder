input_file="nlwp.txt"
out_file="avgNlwp.txt"
column_number=1
sum=0
value=0
while read -r line; do
    # Use awk to extract the value from the specified column
    value=$(echo "$line" | awk -v col="$column_number" '{print $col}')
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