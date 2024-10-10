# BI-PA1 repo

In here I'll be putting my solutions to 2024 CTU Programming 1 course homeworks. I'll try making solutions both in Rust and C.

## Building and usage

### C

Just navigate to a given folder and use compiler of your choice to compile the .c files.

```
cd hw00
g++ -o output.out -Wall -pedantic *.c
```

Each folder is supplied with testing data from the uni in the `tests` folder. Each _\_in.txt is paired with and _\_out.txt so you can test the program.

```
cd hw00
g++ -o output.out -Wall -pedantic *.c
./output.out < tests/0000_in.txt > tmp && diff tmp tests/0000_out.txt
```

I also have a script for a more conveninent way of testing, but because this repo is also a subject to our GIT course (which makes some unusual choices about repo structure), I can't put a test script in the root dir. So for the convenience of anyone cloning this I'll be putting the script at the end of this readme. May God have mercy.

If you put that script in the root you can run any of the homeworks againts all supplied tests like so

```
chmod +x test.sh
cd hw00
../test.sh
```

### Rust

Idk yet :D

## test.sh

```
#!/bin/bash

TESTFILES=("tests"/*)
g++ -o output -Wall -pedantic *.c

for FILE in "${TESTFILES[@]}"; do
  FILENAME=$(basename "$FILE")

  # Split type_id into type and id
  IFS='_' read -r ID TYPE <<< "$FILENAME"

  if [ "$TYPE" == "out.txt" ]; then
    continue
  fi

  OUTPUT_FILE="tests/""$ID""_out.txt"

  # Check if the output file exists
  if [ ! -f "$OUTPUT_FILE" ]; then
    echo "Warning: Output file for $FILENAME not found. The heck?"
    continue
  fi

  ./output < "$FILE" > tmp || true
  if diff tmp "$OUTPUT_FILE"; then
    echo Test $ID passsed
  else
    echo "Test $ID failed"
    diff tmp "$OUTPUT_FILE"
    rm tmp
    exit 1
  fi
done
rm output
rm tmp
exit 0
```
