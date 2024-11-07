#!/bin/bash

# Initialize variables
diff_count=0
test_num=1

# Generate random positive integers for data
generate_random_positive() {
    echo $((RANDOM % 100000))
}

# Generate the structured list part of the input
generate_list_part() {
    list="{"
    num_cells=$1
    for ((i=0; i<num_cells; i++)); do
        a=$(generate_random_positive)
        b=$((a + RANDOM % 100000))  # Ensure b >= a
        c=$(generate_random_positive)
        ((c=c+1))
        d=$(generate_random_positive)
        list+="[$a-$b,$c,$d]"
        if [ "$i" -lt $((num_cells - 1)) ]; then
            list+=","
        fi
    done
    list+="}"
    echo "$list"
}

# Generate the integer pairs part of the input
generate_pairs_part() {
    num_pairs=$1
    pairs=""
    for ((i=0; i<num_pairs; i++)); do
        x=$(generate_random_positive)
        y=$(generate_random_positive)
        pairs+="$x $y"$'\n'
    done
    echo "$pairs"
}

# Loop to generate data and compare outputs until 5 differences are found
while [ "$diff_count" -lt 1 ]; do
    # Increment the number of cells and pairs as test progresses
    num_cells=$((test_num * 4))  # Increase cell count with each test
    num_pairs=$((test_num * 300))  # Increase pairs count with each test

    # Generate input data
    list_part=$(generate_list_part "$num_cells")
    pairs_part=$(generate_pairs_part "$num_pairs")
    
    # Prepare input file
    input_file=$(printf "tests/%03d0_in.txt" "$test_num")
    echo -e "${list_part}\n${pairs_part}" > "$input_file"

    # Run the reference and attempt programs
    ref_output=$(printf "tests/%03d0_out.txt" "$test_num")
    attempt_output="tmp.txt"
    
    ./reference.out < "$input_file" > "$ref_output"
    ./a.out < "$input_file" > "$attempt_output"
    
    # Compare outputs
    if ! diff -q "$ref_output" "$attempt_output" > /dev/null; then
        echo "Difference found in test #$test_num"
        ((diff_count++))
    fi
    rm tmp.txt

    # Increment test number for next iteration
    # ((test_num++))
done

echo "Script completed with $diff_count differences found."
