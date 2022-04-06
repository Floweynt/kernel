inline void lidt(void* idt) { asm volatile("lidtq %0" : : "m"(idt)); }

inline void lgdt(void* gdt) { asm volatile("lgdtq %0" : : "m"(gdt)); }
