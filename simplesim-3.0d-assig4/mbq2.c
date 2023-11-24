#include <stdio.h>
#include <stdlib.h>
#define whileItr 16384
int main() {
    int array[512000];
    int i = 0, idx = 0;
    while(1){
        for (i = 0; i < 512000; i=i+512) {
            array[i] = i;
        }
        if(idx > 10000) break;
        idx++;
    }
}

// In this scenario, we accessed every 512th element. Considering that the cache line 
// spans 64 bytes, this approach involves accessing data from every eighth line
// . For this particular microbenchmark, the next-line prefetcher was not effective, 
// while the stride prefetcher demonstrated a low rate of cache misses. 