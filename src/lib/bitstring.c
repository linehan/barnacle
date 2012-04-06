#include <stdio.h>
#include "bitfit.h"

/*
 * The CPUID instruction takes no parameters as CPUID implicitly uses 
 * the EAX register. The EAX register should be loaded with a value 
 * specifying what information to return. 
 *
 * CPUID should be called with EAX = 0 first, as this will return the 
 * highest calling parameter that the CPU supports. To obtain extended 
 * function information CPUID should be called with bit 31 of EAX set. 
 * To determine the highest extended function calling parameter, call 
 * CPUID with EAX = 80000000h.
 *
 * EAX=0: Get vendor ID --------------------------------------------------------
 * This returns the CPU's manufacturer ID string - a twelve character ASCII 
 * string stored in EBX, EDX, ECX - in that order. The highest basic calling 
 * parameter (largest value that EAX can be set to before calling CPUID) is 
 * returned in EAX.
 *
 * EAX=1: Processor Info and Feature Bits --------------------------------------
 * This returns the CPU's stepping, model, and family information in EAX 
 * (also called the signature of a CPU), feature flags in EDX and ECX, and 
 * additional feature info in EBX.
 *
 * The format of the information in EAX is as follows:
 *
 *       3:0  - Stepping
 *       7:4  - Model
 *      11:8  - Family
 *      13:12 - Processor Type
 *      19:16 - Extended Model
 *      27:20 - Extended Family
 *
 * EAX=1 CPUID feature bits table
 *
 * EDX Register 
 *
 * Bit 	Short       Feature
 * 0 	fpu 	    Onboard x87 FPU 	
 * 1 	vme 	    Virtual mode extensions 
 * 2 	de 	    Debugging extensions  	
 * 3 	pse 	    Page size extensions 	
 * 4 	tsc 	    Time Stamp Counter 	
 * 5 	msr 	    Model-specific registers
 * 6 	pae 	    Physical Address Extension
 * 7 	mce 	    Machine Check Exception 	
 * 8 	cx8 	    CMPXCHG8 (compare-and-swap) instruction 	
 * 9 	apic 	    Onboard Advanced Programmable Interrupt Controller 	
 * 10 	---         (reserved) 
 * 11 	sep 	    SYSENTER and SYSEXIT instructions 	
 * 12 	mtrr 	    Memory Type Range Registers 	
 * 13 	pge 	    Page Global Enable bit in CR4 
 * 14 	mca 	    Machine check architecture 
 * 15 	cmov 	    Conditional move and FCMOV instructions 	
 * 16 	pat 	    Page Attribute Table 	
 * 17 	pse36 	    36-bit page huge pages 
 * 18 	pn 	    Processor Serial Number 	
 * 19 	clflush     CLFLUSH instruction (SSE2) 	
 * 20 	---         (reserved) 	
 * 21 	dts 	    Debug store: save trace of executed jumps 	
 * 22 	acpi 	    Onboard thermal control MSRs for ACPI 	
 * 23 	mmx 	    MMX instructions 	
 * 24 	fxsr 	    FXSAVE, FXRESTOR instructions, 
 * 25 	sse 	    SSE instructions (a.k.a. Katmai New Instructions) 	
 * 26 	sse2 	    SSE2 instructions 	
 * 27 	ss 	    CPU cache supports self-snoop 	
 * 28 	ht 	    Hyper-threading 	
 * 29 	tm 	    Thermal monitor automatically limits temperature 	
 * 30 	ia64 	    IA64 processor emulating x86 	
 * 31 	pbe 	    Pending Break Enable (PBE# pin) wakeup support 	
 * 
 * ECX Register
 * 
 * 0 	pni 	    Prescott New Instructions (SSE3)
 * 1 	pclmulqdq   PCLMULQDQ support
 * 2 	dtes64 	    64-bit debug store (edx bit 21)
 * 3 	monitor     MONITOR and MWAIT instructions (SSE3)
 * 4 	ds_cpl 	    CPL qualified debug store
 * 5 	vmx 	    Virtual Machine eXtensions
 * 6 	smx 	    Safer Mode Extensions (LaGrande)
 * 7 	est 	    Enhanced SpeedStep
 * 8 	tm2 	    Thermal Monitor 2
 * 9 	ssse3 	    Suplemental SSE3 instructions
 * 10 	cid 	    Context ID
 * 11 	---         (reserved)
 * 12 	fma 	    Fused multiply-add (FMA3)
 * 13 	cx16 	    CMPXCHG16B instruction
 * 14 	xtpr 	    Can disable sending task priority messages
 * 15 	pdcm 	    Perfmon & debug capability
 * 16 	---         (reserved)
 * 17 	pcid 	    Process context identifiers (CR4 bit 17)
 * 18 	dca 	    Direct cache access for DMA writes[8][9]
 * 19 	sse4_1 	    SSE4.1 instructions
 * 20 	sse4_2 	    SSE4.2 instructions
 * 21 	x2apic 	    x2APIC support
 * 22 	movbe 	    MOVBE instruction (big-endian, Intel Atom only)
 * 23 	popcnt 	    POPCNT instruction
 * 24 	tscdeadline APIC supports one-shot operation using a TSC deadline value
 * 25 	aes 	    AES instruction set
 * 26 	xsave 	    XSAVE, XRESTOR, XSETBV, XGETBV
 * 27 	osxsave     XSAVE enabled by OS
 * 28 	avx 	    Advanced Vector Extensions
 * 29 	f16c 	    CVT16 instruction set (half-precision) FP support
 * 30 	rdrnd 	    RDRAND (on-chip random number generator) support
 * 31 	hypervisor  Running on a hypervisor (always 0 on a real CPU)
 *
 *
 * EAX=2: Cache and TLB Descriptor information ---------------------------------
 * This returns a list of descriptors indicating cache and TLB capabilities 
 * in EAX, EBX, ECX and EDX registers.
 * 
 * EAX=3: Processor Serial Number ----------------------------------------------
 * Note that the processor serial number feature must be enabled in the BIOS 
 * setting in order to function.
 *
 * EAX=80000000h: Get Highest Extended Function Supported ----------------------
 * The highest calling parameter is returned in EAX.
 *
 * EAX=80000001h: Extended Processor Info and Feature Bits ---------------------
 * This returns extended feature flags in EDX and ECX.
 *
 * EAX=80000002h,80000003h,80000004h: Processor Brand String -------------------
 * These return the processor brand string in EAX, EBX, ECX and EDX. CPUID must 
 * be issued with each parameter in sequence to get the entire 48-byte 
 * null-terminated ASCII processor brand string. It is necessary to check 
 * whether the feature is supported by the CPU by issuing CPUID with 
 * EAX = 80000000h first and checking if the returned value is greater or equal 
 * to 80000004h.
 *
 * EAX=80000005h: L1 Cache and TLB Identifiers ---------------------------------
 * This function contains the processor’s L1 cache and TLB characteristics.
 *
 * EAX=80000006h: Extended L2 Cache Features -----------------------------------
 * Returns details of the L2 cache in ECX, including the line size in bytes, 
 * type of associativity (encoded by a 4 bits) and the cache size.
 *
 * EAX=80000007h: Advanced Power Management Information ------------------------
 * This function provides advanced power management feature identifiers.
 *
 * EAX=80000008h: Virtual and Physical address Sizes ---------------------------
 * Returns largest virtual and physical address sizes in EAX.
 *
 */

void cpuid(unsigned info, unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx)
{
        *eax = info;
        __asm volatile
        ("mov %%ebx, %%edi;" /* 32bit PIC: don't clobber ebx */
        "cpuid;"
        "mov %%ebx, %%esi;"
        "mov %%edi, %%ebx;"
        :"+a" (*eax), "=S" (*ebx), "=c" (*ecx), "=d" (*edx)
        : :"edi");
}
 
/*
 * Print the outputs of cpuid
 */
void stat_cpuid(void)
{
        unsigned int eax, ebx, ecx, edx;
        int i;
 
        for (i=0; i<6; ++i) {
                cpuid(i, &eax, &ebx, &ecx, &edx);
                printf("EAX=%i: %#010x %#010x %#010x %#010x\n", i, eax, ebx, ecx, edx);
        }
}



/*
 * ones32 -- Compute the number of set bits (ones) in a 32-bit integer v
 * @v: unsigned 32-bit integer value
 */
unsigned ones32(register uint32_t x)
{
        /* 32-bit recursive reduction using SWAR...
	 * but first step is mapping 2-bit values
	 * into sum of 2 1-bit values in sneaky way
	 */
        x -= ((x >> 1) & 0x55555555);
        x  = (((x >> 2) & 0x33333333) + (x & 0x33333333));
        x  = (((x >> 4) + x) & 0x0f0f0f0f);
        x += (x >> 8);
        x += (x >> 16);
        return(x & 0x0000003f);
}




/* 
 * lzc -- Return the number of leading zeroes in a 32-bit value v 
 *  @v: unsigned 32-bit integer value
 */
unsigned lzc(uint32_t v)
{
        v |= (v >> 1);
        v |= (v >> 2);
        v |= (v >> 4);
        v |= (v >> 8);
        v |= (v >> 16);

        return (WORDBITS - ones32(v));
}




/*
 * uns -- Return the number of the first unset bit encountered
 *
 *   1. Invert the number
 *   2. Compute the two's complement of the inverted number
 *   3. AND the results of (1) and (2)
 *   4. Find the position by computing the binary logarithm of (3)
 *      e.g.
 *      For the number 10110111:
 *              1. 01001000 `------- first zero
 *              2. 10111000
 *              3. 01001000 AND 10111000 = 00001000
 *              4. log2(00001000) = 3
 *                           `------- clever girl
 *
 * @v: unsigned 32-bit integer value
 */
unsigned ffz(uint32_t v)
{
        unsigned pos = 0;

        __asm__("bsfl %1,%0\n\t"
                "jne 1f\n\t"
                "movl $32, %0\n"
                "1:"
                : "=r" (pos)
                : "r" (~(v)));

        return (pos > WORDBITS-1) ? WORDBITS : (unsigned short)pos;
}



