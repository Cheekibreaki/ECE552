#cp /cad2/ece552f/benchmarks/go.pisa-big .
#cp /cad2/ece552f/benchmarks/2stone9.in .
make sim-safe
/cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc mbq1.c -S
/cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc mbq1.c -O0 -o mbq10
# ./sim-safe -v -max:inst 1000 -dumpconfig config.txt -redir:sim go.simout -redir:prog go.progout mbq10
./sim-safe -dumpconfig config.txt -redir:sim go.simout -redir:prog go.progout mbq10 5
# ./sim-safe -dumpconfig config.txt -redir:sim go.simout -redir:prog go.progout go.pisa-big 50 9 2stone9.in
echo "----------------------------------------------------"
cat go.simout
echo "----------------------------------------------------"
echo "go.simout end"
# echo "----------------------------------------------------"
# cat go.progout
# echo "----------------------------------------------------"
# echo "go.progout end"

# ./sim-safe /cad2/ece552f/benchmarks/gcc.eio