#include <stdio.h>
#include <stdlib.h>
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

// By accessing every 64th byte, this approach aligns with the 64-byte length of a cache line. 
// The prefetcher, designed to automatically load the next cache line, ensures a high likelihood of 
// cache hits when elements in the newly loaded line are accessed. This method significantly
// lowers the cache miss rate compared to situations without such prefetching. 