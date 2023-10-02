#!/bin/bash

# Copying benchmark files
cp /cad2/ece552f/benchmarks/go.pisa-big .
cp /cad2/ece552f/benchmarks/2stone9.in .

# Building simulation
make sim-safe

# Updating PATH variable
export PATH=$PATH:/cad2/ece552f/compiler/bin

# Compilation
/cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc mbq1.c -S

if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

/cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc mbq1.c -O0 -o mbq10

if [ $? -ne 0 ]; then
    echo "Compilation with optimization level 0 failed!"
    exit 1
fi

# Running simulation
sim-safe -dumpconfig config.txt -redir:sim go.simout -redir:prog go.progout mbq10

# Displaying results
echo "----------------------------------------------------"
cat go.simout
echo "----------------------------------------------------"
echo "go.simout end"
echo "----------------------------------------------------"
cat go.progout
echo "----------------------------------------------------"
echo "go.progout end"
