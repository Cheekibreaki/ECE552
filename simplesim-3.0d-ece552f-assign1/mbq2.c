#include <stdio.h>
#include <stdlib.h>

// Frequently used command:
// /cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc mbq1.c -O0 -S -o mbqQ2.s
// /cad2/ece552f/compiler/bin/ssbig-na-sstrix-gcc mbq1.c -O0 -o mbqQ2

// #define LD
#define Q1

int main(int argc, char *argv[]) {

    register int iteration = atoi(argv[1]);
    
    register int r1, r2, r3, r4, r5, r6;
    // register int ra, rb, rc, rd, re;
    register int a, b, c;

    register int i;
    int j;
    int m, n;
    int* o = malloc(sizeof(int));
    int* p = malloc(sizeof(int));
    int* q = malloc(sizeof(int));

    // By inspecting the assembly, we also notice that there's two cycle
    // stall in comparison: i < iteration.
    for (i = 0; i < iteration;) {
        #ifdef Q1
            // EX/MEM hazard: 2 cycle stalls
            // add r2, r3 -> r1
            // add r1, r4 -> r5
            r1 = r4 + r5;
            r3 = r2 + r4;
            r5 = r1 + r4;
            r1 = r3 + r4;

            // MEM/WB hazard: 1 stall
            // add rb, rc -> ra
            // add rb, rc -> rd
            // add ra, rb -> rb
            // ra = rb + rc;
            // rd = ra + rc;
            // rc = ra + rb;
            // rc = rd + rb;

            // ra = ra + ra;


        #else 

        #ifdef LD
            // For Q2, since MIPS only has 32 register, we have to test section
            // by section, this section is instruction that begins with ld
            // Result: we should have three 2 cycle stall as constructed below
            // 1 cycle stall for comparison
            // slt	$2,$22,$16
	        // bne	$2,$0,$L5

            // ld + arithmetic 2 cycle stall
            // lw	$2,16($fp)
	        // addu	$17,$2,2
            r1 = j + 2;

            // eliminate dependency
            b = a; c = a; 
            
            // ld + ld: 2 cycle stall
            // lw	$2,32($fp)
	        // lw	$18,0($2)
            r2 = *(p); 

            // eliminate dependency
            b = a; c = a;
            
            // ld + st (data depends on ld value): no stall
            // lw	$2,28($fp)
	        // sw	$2,20($fp)
            m = (int)o;

            // eliminate dependency
            b = a; c = a;

            // ld + st (address depends on ld value): 2 cycle stall
            // lw	$2,36($fp)
	        // sw	$17,0($2)
            *q = r1;

            // eliminate dependency
            b = a; c = a;
        #endif

        #ifdef ARITH
            // For Q2, since MIPS only has 32 register, we have to test section
            // by section, this section is instruction that begins with arthimatic
            // Result: we should have three 1 cycle stall as constructed below
            // 1 cycle stall for comparison
            // slt	$2,$22,$16
	        // bne	$2,$0,$L5

            // arithmatic + arithmatic: 1 cycle stall
            // addu	$17,$18,1
	        // addu	$18,$17,1
            r1 = r2 + 1;
            r2 = r1 + 1;

            // eliminate dependency
            b = a; c = a;

            // arithmatic + ld: 1 cycle stall
            // addu	$2,$17,$18
	        // lw	$18,0($2)
            r2 = *(int*)(r1 + r2);

            // eliminate dependency
            b = a; c = a;

            // arithmatic + st (data depends on arith value): no stall
            // addu	$2,$17,$19
	        // sw	$2,0($18)
            *(int *)(r2) = (r1 + r3);

            // eliminate dependency
            b = a; c = a;

            // arithmatic + st (address depends on arith value): 1 cycle stall
            // addu	$2,$17,$18
	        // sw	$17,0($2)
            *(int *)(r1 + r2) = r1;
        #endif
        #endif
        i++;
        asm("nop");
        asm("nop");
        asm("nop");
    }

    return 0;
}