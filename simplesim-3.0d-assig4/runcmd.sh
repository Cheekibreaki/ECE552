# make clean
make sim-cache

./sim-cache -redir:sim compress_nextline.simout -config cache-config/cache-lru-nextline.cfg /cad2/ece552f/benchmarks/compress.eio 
./sim-cache -redir:sim gcc_nextline.simout -config cache-config/cache-lru-nextline.cfg /cad2/ece552f/benchmarks/gcc.eio 
./sim-cache -redir:sim go_nextline.simout -config cache-config/cache-lru-nextline.cfg /cad2/ece552f/benchmarks/go.eio 

./sim-cache -redir:sim compress_stride.simout -config cache-config/cache-lru-stride.cfg /cad2/ece552f/benchmarks/compress.eio 
./sim-cache -redir:sim gcc_stride.simout -config cache-config/cache-lru-stride.cfg /cad2/ece552f/benchmarks/gcc.eio
./sim-cache -redir:sim go_stride.simout -config cache-config/cache-lru-stride.cfg /cad2/ece552f/benchmarks/go.eio

# ./sim-cache -redir:sim compress_open.simout -config cache-config/cache-lru-open.cfg /cad2/ece552f/benchmarks/compress.eio
# ./sim-cache -redir:sim gcc_open.simout -config cache-config/cache-lru-open.cfg /cad2/ece552f/benchmarks/gcc.eio
# ./sim-cache -redir:sim go_open.simout -config cache-config/cache-lru-open.cfg /cad2/ece552f/benchmarks/go.eio