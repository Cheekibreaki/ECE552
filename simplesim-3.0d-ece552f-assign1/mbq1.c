#include <stdio.h>
// /cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc mbq1.c -O2 -o mbq1
// use optimization flag 0
int main (int argc, char *argv[ ])
{
    register int i = 0;
    // pass from argument into the while loop to eliminate any problem cased by directly using while (i < 10000)
    // might cause extra stalls and dependences
    register int X_ITER = atoi(argv[1]);
    // set variables as registers
    register int a = 1, b = 1, c = 1;

    // below are the variables used for load to use case
    // select "a" from the stack memory to load
    // int a = 1;
    // register int b = 0, c = 0;
    while (i < X_ITER) {
        // // two cycle dependeces
        a += 1;
        b += a;
        
        // // counter increment
        i += 1;
        // // one cycle dependeces
        // // insert an unrelated instns. in between two related instns.
        a += 1;
        c += 1;
        b += a;

        // add a load case for Q2 load to use reference, uncomment if necessary
        // c = a;
        // b = c + 1;
    }
    return 0;
}

/*
	move	$17,$2
	li	$3,0x00000001		# 1
	li	$4,0x00000001		# 1
	li	$5,0x00000001		# 1
$L2:
	slt	$2,$16,$17
	bne	$2,$0,$L4
	j	$L3
$L4:
	addu	$3,$3,1
	addu	$4,$4,$3
	addu	$16,$16,1
	addu	$3,$3,1
	addu	$5,$5,1
	addu	$4,$4,$3
	j	$L2
$L3:
	move	$2,$0
	j	$L1

THIS IS FOR QUESTION 1
so for one iteration of the while loop, we will expect:
1x 2-cycle stall from the while loop itself
1x 2-cycle stall from the forced 2 cycle dependencies
1x 1-cycle stall from the forced 1 cycle dependency
*/

/*
	move	$3,$0
	move	$4,$0
$L2:
	slt	$2,$16,$17
	bne	$2,$0,$L4
	j	$L3
$L4:
	lw	$4,16($fp)
	addu	$3,$4,1
	j	$L2

THIS IS FOR QUESTION 2
same while loop as above, the only thing changed is the content within the while
for this while loop, we are expecting:
1x 1-cycle stall from the while loop
1x 2-cycle stall from the load to use
*/
























// #include <stdio.h>
// // #define P2_BENCHMARKS

// int globvar = 1;
// int  main (int argc, char *argv[])
// {
//     register int res = 0, r1 = 1, r2 = 1, r3 = 0;
//     register int r_const = 16384;

//     register int i = 0;
//     register int independent = 0;
//     register int iter = 10000000;

//     switch(atoi(argv[1])){
//         case 1:
//             /*
//             * Case1. for 2 stalls
//             * lw	$1,globvar
//             * addu	$2,$1,$3
//             */
//             while(i < iter){
//                 r1 = globvar;
//                 res = r1 + iter;
//                 i++;
//                 asm("nop");
//                 asm("nop");
//             }
//             break;
//         case 2:
//             /* 
//             * Case2. for 1 stall 
//             * lw	$1,globvar
//             * move	$5,$8
//             * addu	$3,$1,$9
//             */
//             while(i < iter){
//                 r1 = globvar;
//                 r2 = independent;
//                 res = r1 + iter;
//                 i++;
//                 asm("nop");
//                 asm("nop");
//             }
//             break;
//         case 3:
//             /* 
//             * Case3. for 2 data stalls with propogated stall 
//             * lw	$1,globvar
//             * addu	$3,$1,$10
//             * addu	$5,$1,$10
//             * addu	$6,$1,$10
//             */
//             while(i < iter){
//                 r1 = globvar;
//                 res = r1 + iter;
//                 r2 = r1 + iter;
//                 r3 = r1 + iter;
//                 i++;
//                 asm("nop");
//                 asm("nop");
//             }
//             break;
//     }

// // #ifdef P2_BENCHMARKS
// //     /* ALU operations */
// //     // while(i < iter){
// //     //     res = r1 + r2;

// //     //     /*ALU+STval 0 stall*/
// //     //     // globvar = res;

// //     //     /*ALU+STaddr 1 stall*/
// //     //     // *(int *)(res) = r_const;

// //     //     /*ALU+ALU 1 stall*/
// //     //     // r3 = res + r1;

// //     //     /*ALU+LWaddr 1 stall*/
// //     //     // r3 = *(int *)(res);
// //     //     i++;
// //     //     asm("nop");
// //     //     asm("nop");
// //     // }

// //     /* Load Operations */
// //     while(i < iter){
// //         res = globvar;

// //         /*LW+STval 0 stall*/
// //         // globvar = res;

// //         /*LW+STaddr 2 stalls*/
// //         *(int *)(res) = r_const;

// //         /*LW+ALU 2 stalls*/
// //         // r3 = res+r1;

// //         /*LW+LWaddr 2 stalls*/
// //         // r3 = *(int *)(res);
// //         i++;
// //         asm("nop");
// //         asm("nop");
// //     }
// // #endif

//     return 0;
// }
