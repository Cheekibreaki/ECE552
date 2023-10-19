#include <stdio.h>

int  main (int argc, char *argv[])
{
    register int r0 = 0, r1 = 0, r2 = 0;
    register int iter = 1000;
    register int globalvar = 1;

    r1 = r0 + iter;
    r2 = r0 + r0;
    
    register int r0 = 0, r1 = 0, r2 = 0;

    r1 = r0 + 1000;
    r2 = r0 + r0;

    do{
        if(r2 == 0){
            r2 = r0 + 1;
        }
        r2 = r0 + r0;
        r1--;
    }while(r1 != 0);

    return 0;
}
