__asm__("ljmpl $0x0008, $main\n");

#include "lib/lib.h"
#include "interrupts/idt.h"

int main ()
{
	__asm__ __volatile__("sti");
	printf("[INFO]: Loaded kernel\n");
	printf("[INFO]: Install IDT\n");
	init_idt();

	register_idt([]() {
		ENTER_IRQ;

		static int count = 0;
		if(count == 100)
		{
			printf("timer interrupt\n");
			count = 0;
		}
		count++;

		EXIT_IRQ;
	}, 0b10101110, 0);

	register_idt([]() {
		ENTER_IRQ;
		printf("kernel double-faulted!\n");
		EXIT_IRQ;
	}, 0b10101110, 0x8);

	register_idt([]() {
		ENTER_IRQ;
		printf("general protection fault!\n");
		EXIT_IRQ;
	}, 0b10101110, 0xd);


	install_idt();

	printf("[INFO]: Dumping video data:\n");
	printf("[INFO]: Screen resolution: %dpx wide, %dpx high\n", VBE_MODE_PTR->width, VBE_MODE_PTR->height);
	printf("[INFO]: %d lines, %d chars per line (CPL)\n", get_max_lines(), get_max_chars());
	printf("[INFO]: VESA controller version: %u(%#6.4x), using %u * 64KiB\n",
		(unsigned int)VBE_INFO_PTR->vbe_version / 0x100,
		(unsigned int)VBE_INFO_PTR->vbe_version,
		(unsigned int)VBE_INFO_PTR->total_memory
	);

    while(1) {}
}
