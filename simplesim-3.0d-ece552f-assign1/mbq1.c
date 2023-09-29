#include <stdio.h>

int globvar = 1;

int  main (int argc, char *argv[])
{
    register int r1, res;
    register int r_const = 16384;

    register int i = 0;
    register int iter = 10000000;
    // while(i < iter){
    //     r1 = globvar;
    //     res = r1 + iter;
    //     i++;
    //     asm("nop");
    //     asm("nop");
    // }

    
    return 0;
}