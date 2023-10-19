make clean > "make.log"
make > "make.log"
# ./predictor /cad2/ece552f/cbp4_benchmarks/astar.cbp4.gz
# ./predictor /cad2/ece552f/cbp4_benchmarks/bwaves.cbp4.gz
# ./predictor /cad2/ece552f/cbp4_benchmarks/bzip2.cbp4.gz
# ./predictor /cad2/ece552f/cbp4_benchmarks/gcc.cbp4.gz
# ./predictor /cad2/ece552f/cbp4_benchmarks/gromacs.cbp4.gz
# ./predictor /cad2/ece552f/cbp4_benchmarks/hmmer.cbp4.gz
# ./predictor /cad2/ece552f/cbp4_benchmarks/mcf.cbp4.gz
# ./predictor /cad2/ece552f/cbp4_benchmarks/soplex.cbp4.gz
# Output file for saving results
output_file="output.txt"
> "$output_file"
# List of input files
files=(
    "/cad2/ece552f/cbp4_benchmarks/astar.cbp4.gz"
    "/cad2/ece552f/cbp4_benchmarks/bwaves.cbp4.gz"
    "/cad2/ece552f/cbp4_benchmarks/bzip2.cbp4.gz"
    "/cad2/ece552f/cbp4_benchmarks/gcc.cbp4.gz"
    "/cad2/ece552f/cbp4_benchmarks/gromacs.cbp4.gz"
    "/cad2/ece552f/cbp4_benchmarks/hmmer.cbp4.gz"
    "/cad2/ece552f/cbp4_benchmarks/mcf.cbp4.gz"
    "/cad2/ece552f/cbp4_benchmarks/soplex.cbp4.gz"
)

# Function to run the predictor command and save output to the file
run_predictor() {
    predictor_command="./predictor $1"
    $predictor_command >> "$output_file"
}

# Run the predictor commands in parallel
for file in "${files[@]}"; do
    run_predictor "$file" &
done

# Wait for all background processes to finish
wait

# Use grep to extract lines matching the pattern and calculate the average
average=$(grep "openend: MISPRED_PER_1K_INST" output.txt | awk -F '[: ]+' '{ sum += $4; count++ } END { print sum / count }')

echo "$average"
# if [ -n "$average" ]; then
#     echo "Average MISPRED_PER_1K_INST: $average"
# else
#     echo "No MISPRED_PER_1K_INST values found in the output."
# fi






# #include <iostream>
# #define PBTSIZE 8
# using namespace std;
# static int HistoryLength [PBTSIZE] = {0,2,4,8,16,32,64,128};      // Take short OR long History to hash
# static int AddressLength [PBTSIZE] = {8,8,9,11,12,12,12,14};
# static int NBitCounter [PBTSIZE] = {5,5,5,5,4,4,4,4};
# int main()
# {
#     int result = 0;

#     for (int i = 0; i < PBTSIZE; i++) {
        
#         int power = (1 << AddressLength[i]) * NBitCounter[i];
    
#         result += power;
#     }
#     result+=HistoryLength[(sizeof(HistoryLength) / sizeof(HistoryLength[0])) - 1];
#     cout << result  << endl;
#     cout << (result > 131072)  << endl;
#     return 0;
# }
