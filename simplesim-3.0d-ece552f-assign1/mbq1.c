#include <stdio.h>
// #define P2_BENCHMARKS

int globvar = 1;
int  main (int argc, char *argv[])
{
    register int res = 0, r1 = 1, r2 = 1, r3 = 0, r4, r5;

    register int i = 0;
    register int iter = 100000;

    switch(atoi(argv[1])){
        case 1:
        /*two stall*/
            while(i < iter){
                r1 = globvar;
                res = r1 + iter;
                i++;
                asm("nop");
                asm("nop");
            }
            break;
        case 2:
            /* 
            * Case2. for one stall 
            * lw	$1,globvar
            * move	$5,$8
            * addu	$3,$1,$9
            */
            while(i < iter){
                r1 = globvar;
                r2 = r3;
                res = r1 + iter;
                i++;
                asm("nop");
                asm("nop");
            }
            break;
        case 3:
            /* 
            * Case3. for 2 data stalls with propogated stall 
            * lw	$1,globvar
            * addu	$3,$1,$10
            * addu	$5,$1,$10
            * addu	$6,$1,$10
            */
            while(i < iter){
                r1 = globvar;
                res = r1 + iter;
                r2 = r1 + iter;
                r3 = r1 + iter;
                i++;
                asm("nop");
                asm("nop");
            }
            break;
        case 4:
        /*one stall + two stall*/
            while(i < iter){
                r1 = r4 + r5;
                r3 = r2 + r4;
                r5 = r1 + r4;
                r1 = r3 + r5;
                i++;
                asm("nop");
                asm("nop");
            }
            break;
        case 5:
        /*one stall + one stall(propogated)*/
            while(i < iter){
                r1 = r4 + r5;
                r3 = r2 + r4;
                r5 = r1 + r4;
                r1 = r3 + r4;
                i++;
                asm("nop");
                asm("nop");
            }
            break;
        case 6:
        /*Piority Question*/
            while(i < iter){
                r1 = r4 + r5;
                r3 = r2 + r4;
                r5 = r1 + r3;
                i++;
                asm("nop");
                asm("nop");
            }
            break;
        case 7:
        /*two stalls + one Propogate stall*/
            while(i < iter){
                r1 = r4 + r5;
                r3 = r1 + r4;
                r5 = r1 + r3;
                i++;
                asm("nop");
                asm("nop");
            }
            break;
    }

#ifdef P2_BENCHMARKS
    /* ALU operations */
    // while(i < iter){
    //     res = r1 + r2;

    //     /*ALU+STval 0 stall*/
    //     // globvar = res;

    //     /*ALU+STaddr 1 stall*/
    //     // *(int *)(res) = r_const;

    //     /*ALU+ALU 1 stall*/
    //     // r3 = res + r1;

    //     /*ALU+LWaddr 1 stall*/
    //     // r3 = *(int *)(res);
    //     i++;
    //     asm("nop");
    //     asm("nop");
    // }

    /* Load Operations */
    while(i < iter){
        res = globvar;

        /*LW+STval 0 stall*/
        // globvar = res;

        /*LW+STaddr 2 stalls*/
        *(int *)(res) = r_const;

        /*LW+ALU 2 stalls*/
        // r3 = res+r1;

        /*LW+LWaddr 2 stalls*/
        // r3 = *(int *)(res);
        i++;
        asm("nop");
        asm("nop");
    }
#endif

    return 0;
}
