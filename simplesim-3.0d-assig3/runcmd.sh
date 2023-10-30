make sim-safe
sim-safe -max:inst 1000000 -redir:sim gcc.simout -redir:prog gcc.progout /cad2/ece552f/benchmarks/gcc.eio
sim-safe -max:inst 1000000 -redir:sim go.simout -redir:prog go.progout /cad2/ece552f/benchmarks/go.eio
sim-safe -max:inst 1000000 -redir:sim compress.simout -redir:prog compress.progout /cad2/ece552f/benchmarks/compress.eio

echo "----------------------------------------------------"
cat go.simout
echo "----------------------------------------------------"
echo "go.simout end"