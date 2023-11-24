#include <stdio.h>
#include <stdlib.h>
#define arraySize 1000000
#define whileItr 16384
int main() {
    int array[arraySize];
    int i = 0, idx = 0;
    while(true){
        for (i = 0; i < arraySize; i=i+64) {
            array[i] = 114514;
        }
        if(idx > whileItr) break;
        idx++;
    }
}

// By accessing every 64th byte, this approach aligns with the 64-byte length of a cache line. 
// The prefetcher, designed to automatically load the next cache line, ensures a high likelihood of 
// cache hits when elements in the newly loaded line are accessed. This method significantly
// lowers the cache miss rate compared to situations without such prefetching. 