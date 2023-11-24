#include <stdio.h>
#include <stdlib.h>
#define whileItr 16384
int main() {
    int array[512000];
    int i = 0, idx = 0, idx1;
    while(1){
        for (i = 0; i < 512000; i=i+512) {
            array[i%32] = i;
            array[i%31] = i;
        }
        if(idx > 10000) break;
        idx++;
    }
}

// In this scenario, we accessed every 512th element. Considering that the cache line 
// spans 64 bytes, this approach involves accessing data from every eighth line. 
// how ever as we add a change in the access sequence in this case i%32, there will be 
// a stage change back to init, which cause the stride in RPT change, thus will lead to
// a miss prediction in in the stride-prefetcher