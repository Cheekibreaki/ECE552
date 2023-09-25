#cp /cad2/ece552f/benchmarks/go.pisa-big .
#cp /cad2/ece552f/benchmarks/2stone9.in .
make sim-safe
sim-safe -dumpconfig config.txt -redir:sim go.simout -redir:prog go.progout go.pisa-big 50 9 2stone9.in
echo "----------------------------------------------------"
cat go.simout
echo "----------------------------------------------------"
echo "go.simout end"
echo "----------------------------------------------------"
cat go.progout
echo "----------------------------------------------------"
echo "go.progout end"