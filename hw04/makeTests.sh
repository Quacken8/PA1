#!/bin/bash

diff_count=0
test_num=6

generate_random_positive() {
    echo $(shuf -i 0-2147483647 -n 1 )
}

generate_vehicles() {
    list="{"
    num_vehicles=$1
    for ((i=0; i<num_vehicles; i++)); do
        a=$(generate_random_positive)
        b=$((a + generate_random_positive % (2147483647 - a / 3))) 
        c=$(generate_random_positive % 50)
        ((c=c+1))
        d=$(generate_random_positive % 50)
        list+="[$a-$b,$c,$d]"
        if [ "$i" -lt $((num_vehicles - 1)) ]; then
            list+=","
        fi
    done
    list+="}"
    echo "$list"
}

generate_orders() {
    num_orders=$1
    pairs=""
    for ((i=0; i<num_orders; i++)); do
        x=$(generate_random_positive)
        y=$(generate_random_positive)
        pairs+="$x $y"$'\n'
    done
    echo "$pairs"
}

while [ "$diff_count" -lt 1 ]; do
    num_vehicles=$((100001)) 
    num_orders=$((10000))  

    list_part=$(generate_vehicles "$num_vehicles")
    pairs_part=$(generate_orders "$num_orders")
    
    input_file="tests/1000_in.txt"
    echo -e "${list_part}\n${pairs_part}" > "$input_file"

    ref_output="tests/1000_out.txt"
    attempt_output="tmp.txt"
    
    ./a.out < "$input_file" > "$attempt_output"
    ./reference.out < "$input_file" > "$ref_output"
    
    if ! diff -q "$ref_output" "$attempt_output" > /dev/null; then
        echo "Difference found in test #$test_num"
        ((diff_count++))
    fi
    #rm tmp.txt

    echo "Test #$test_num with $num_vehicles cells and $num_orders pairs completed."

    ((test_num++))
done

echo "Script completed with $diff_count differences found."
