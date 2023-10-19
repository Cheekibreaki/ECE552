import random
import math
import subprocess
import time

def generate_header_file(long_history, short_history):
    header_code = f'''// Updated values based on simulated annealing
int LongHistory[8] = {{ {', '.join(map(str, long_history))} }};
int ShortHistory[8] = {{ {', '.join(map(str, short_history))} }};
'''

    with open('updated_values.h', 'w') as header_file:
        header_file.write(header_code)

def tweak_integer(numbers, max_change):
    index = 0
    change = random.choice([-1, 1])

    while(index == 0):
        index = random.randint(0, len(numbers) - 1)
    
    if((numbers[index] + (change * max_change)) < 0):
        change = 1

    numbers[index] += (change * max_change)

def simulated_annealing_integer(long_history, short_history, max_iterations, initial_temperature, cooling_rate, max_change, shell_script_path):
    best_long_history = long_history[:]
    best_short_history = short_history[:]
    best_cost = 5.5

    for i in range(max_iterations):
        temperature = initial_temperature * (1 - i / max_iterations)
        tweak_integer(long_history, max_change)
        tweak_integer(short_history, max_change)
        generate_header_file(best_long_history, best_short_history)
        time.sleep(0.1)
        # Call the shell script to calculate the cost function
        cost_output = subprocess.check_output(shell_script_path, shell=True, universal_newlines=True)
        new_cost = float(cost_output)

        print("TestLongHistory:", long_history)
        print("TestShortHistory:", short_history)
        print("TestCost:", new_cost)

        if new_cost < best_cost:
            best_cost = new_cost
            best_long_history = long_history[:]
            best_short_history = short_history[:]

    return best_long_history, best_short_history

PBTSIZE = 8
LongHistory = [0, 256, 160, 8, 192, 32, 384, 128]
ShortHistory = [0, 256, 4, 8, 16, 32, 64, 128]

max_iterations = 100000
initial_temperature = 1.0
cooling_rate = 0.001
max_change = 2
shell_script_path = "./runcmd.sh"

best_long_history, best_short_history = simulated_annealing_integer(
    LongHistory, ShortHistory, max_iterations, initial_temperature, cooling_rate, max_change, shell_script_path
)

# cost_output = subprocess.check_output(shell_script_path, shell=True, universal_newlines=True)
# new_cost = float(cost_output)
# print("TestCost:", new_cost)