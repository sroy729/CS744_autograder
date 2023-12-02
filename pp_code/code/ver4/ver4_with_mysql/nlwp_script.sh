output_file="nlwp.txt"
# interval=5
while true; do
    # Use ps to capture process information and append it to the output file
    ps -eLF | grep multiThreadServer | wc >> "$output_file"
    
    # Sleep for the specified interval before taking the next snapshot
    sleep 5
done