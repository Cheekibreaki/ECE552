scons PROTOCOL=MSI CPU_MODELS=TimingSimpleCPU build/ALPHA/gem5.opt

# ./build/ALPHA/gem5.opt ./configs/example/ruby_random_test.py
./build/ALPHA/gem5.opt --debug-flags=ProtocolTrace ./configs/example/ruby_random_test.py