#include <stdio.h>
#include <stdlib.h>
#define arraySize 10000000
#define whileItr 16384
int main() {
    int array[arraySize];
    int i = 0, idx = 0;
    while(true){
        for (i = 0; i < arraySize; i=i+512) {
            array[i] = 114514;
        }
        if(idx > whileItr) break;
        idx++;
    }
}

// In this scenario, we accessed every 512th element. Considering that the cache line 
// spans 64 bytes, this approach involves accessing data from every eighth line
// . For this particular microbenchmark, the next-line prefetcher was not effective, 
// while the stride prefetcher demonstrated a low rate of cache misses. Additionally, 
// varying the increment of 'j' in our tests did not result in any significant 
// change in the miss rate.