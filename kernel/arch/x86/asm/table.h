inline void lidt(void* idt)
{
    asm volatile("lidtq %0" : : "m"(descriptor));
}

inline void lgdt(void* gdt)
{
    asm volatile()
}
