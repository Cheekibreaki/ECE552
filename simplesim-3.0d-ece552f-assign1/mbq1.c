#include <stdio.h>
// #define P2_BENCHMARKS

int globvar = 1;
int  main (int argc, char *argv[])
{
    register int res = 0, r1 = 1, r2 = 1, r3 = 0;
    register int r_const = 16384;

    register int i = 0;
    register int independent = 0;
    register int iter = 10000000;

    switch(atoi(argv[1])){
        case 1:
            /*
            * Case1. for 2 stalls
            * lw	$1,globvar
            * addu	$2,$1,$3
            */
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
            * Case2. for 1 stall 
            * lw	$1,globvar
            * move	$5,$8
            * addu	$3,$1,$9
            */
            while(i < iter){
                r1 = globvar;
                r2 = independent;
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
    }

// #ifdef P2_BENCHMARKS
//     /* ALU operations */
//     // while(i < iter){
//     //     res = r1 + r2;

//     //     /*ALU+STval 0 stall*/
//     //     // globvar = res;

//     //     /*ALU+STaddr 1 stall*/
//     //     // *(int *)(res) = r_const;

//     //     /*ALU+ALU 1 stall*/
//     //     // r3 = res + r1;

//     //     /*ALU+LWaddr 1 stall*/
//     //     // r3 = *(int *)(res);
//     //     i++;
//     //     asm("nop");
//     //     asm("nop");
//     // }

//     /* Load Operations */
//     while(i < iter){
//         res = globvar;

//         /*LW+STval 0 stall*/
//         // globvar = res;

//         /*LW+STaddr 2 stalls*/
//         *(int *)(res) = r_const;

//         /*LW+ALU 2 stalls*/
//         // r3 = res+r1;

//         /*LW+LWaddr 2 stalls*/
//         // r3 = *(int *)(res);
//         i++;
//         asm("nop");
//         asm("nop");
//     }
// #endif

    return 0;
}
