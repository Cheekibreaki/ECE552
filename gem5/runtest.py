# Run it in ***/gem5
# Adjust test_level to enable different tests.

import os
import sys
import time
from random import seed
import random

# test_path = "./build/ALPHA/gem5.opt --debug-flags=RubySlicc ./configs/example/ruby_random_test.py {}"
test_path = "./build/ALPHA/gem5.opt ./configs/example/ruby_random_test.py {}"

seed(time.time())
random_seed = random.randint(0, 2000000)

result_file = open("test_result.txt", "w")

# Simple test: 1
# Simple+Midium test: 2
# Simple+Midium+Hard test: 3
test_level = 1

result_file.write("-------------------Simple Test-------------------\n")
inst_num = 10000

core_num = 1
result_file.write("\nTesting With {} Cores, {} instructions\n".format(core_num, inst_num))
for i in range(10):
  retval = os.system(test_path.format("-n {} -l {} --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

core_num = 2
result_file.write("\nTesting With {} Cores, {} instructions\n".format(core_num, inst_num))
for i in range(10):
  retval = os.system(test_path.format("-n {} -l {} --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

core_num = 4
result_file.write("\nTesting With {} Cores, {} instructions\n".format(core_num, inst_num))
for i in range(10):
  retval = os.system(test_path.format("-n {} -l {} --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

core_num = 8
result_file.write("\nTesting With {} Cores, {} instructions\n".format(core_num, inst_num))
for i in range(10):
  retval = os.system(test_path.format("-n {} -l {} --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

core_num = 16
result_file.write("\nTesting With {} Cores, {} instructions\n".format(core_num, inst_num))
for i in range(10):
  retval = os.system(test_path.format("-n {} -l {} --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

if test_level < 2:
  exit(0)

result_file.write("\n-------------------Midium Test-------------------\n")

core_num = 16
inst_num = 100000

result_file.write("\nTesting With {} Cores, {} instructions. Cache: 8kB-4way\n".format(core_num, inst_num))
for i in range(10):
  retval = os.system(test_path.format("-n {} -l {}  --l1d_size 8kB --l1d_assoc 4 --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

if test_level < 3:
  exit(0)

result_file.write("\n-------------------Hard Test-------------------\n")

core_num = 16
inst_num = 1000000

result_file.write("\nTesting With {} Cores, {} instructions. Cache: 64kB-8way\n".format(core_num, inst_num))
for i in range(15):
  retval = os.system(test_path.format("-n {} -l {}  --l1d_size 64kB --l1d_assoc 8 --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

result_file.write("\nTesting With {} Cores, {} instructions. Cache: 4kB-4way\n".format(core_num, inst_num))
for i in range(15):
  retval = os.system(test_path.format("-n {} -l {}  --l1d_size 4kB --l1d_assoc 4 --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

result_file.write("\nTesting With {} Cores, {} instructions. Cache: 1kB-2way\n".format(core_num, inst_num))
for i in range(15):
  retval = os.system(test_path.format("-n {} -l {}  --l1d_size 1kB --l1d_assoc 2 --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1

result_file.write("\nTesting With {} Cores, {} instructions. Cache: 128B-1way\n".format(core_num, inst_num))
for i in range(15):
  retval = os.system(test_path.format("-n {} -l {}  --l1d_size 128B --l1d_assoc 1 --random_seed={}".format(core_num, inst_num, random_seed)))
  if retval == 0:
    result_file.write("#{} Passed!\n".format(i))
    result_file.flush()
  else:
    result_file.write("#{} Failed! Seed: {}\n".format(i, random_seed))
    result_file.close()
    exit(1)
  random_seed += 1