/// \brief wrapper for the `lidtq` instruction
/// \param idt The IDT to be loaded
inline void lidt(void* idt) { asm volatile("lidtq %0" : : "m"(idt)); }

/// \brief wrapper for the `lgdtq` instruction
/// \param gdt The GDT to be loaded
inline void lgdt(void* gdt) { asm volatile("lgdtq %0" : : "m"(gdt)); }
