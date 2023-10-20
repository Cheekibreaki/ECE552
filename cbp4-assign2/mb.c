#include <stdio.h>

int  main (int argc, char *argv[])
{
    register int r0 = 0, r1 = 0, r2 = 0;

    r1 = r0 + 100000;
    r2 = r0 + r0;
    do{
        if(r2 != 0){
            r2 = r0 + r0;
        }
        else {
            r2 = r0 + 1;
        }
        r1--;
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }while(r1 != 0);

    return 0;
}
