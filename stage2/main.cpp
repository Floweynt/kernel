#pragma pack(1)
#include <cstdint>
asm volatile(".code16gcc");
asm volatile(".code16");
asm volatile("jmpl $0x0000, $main");

struct mmap
{
    uint64_t base;
    uint64_t len;
    uint32_t type;
};

struct alignas(8) dap
{
    uint8_t size = 0x10;
    uint8_t zero = 0;
    uint16_t to_read = 1;
    uint16_t offset = 0x6000;
    uint16_t segment = 0;
    uint32_t start = 9;
    uint32_t unused = 0;
};

struct alignas(8) gdtr
{
    uint16_t size;
    uint32_t base;
};

// i have no clue how gdt works
struct alignas(8) gdt_entry
{
    uint16_t a;
    uint16_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t f;
};

gdt_entry gdt_entries[3] =
{
    {0, 0, 0, 0, 0, 0},
    {0xffff, 0, 0, 0x9a, 0xcf, 0},
    {0xffff, 0, 0, 0x92, 0xcf, 0}
};

gdtr descriptor = { 24, (uint32_t)gdt_entries };

void panic(const char* msg)
{
    while(1) { }
}

uint16_t size;
void do_e820()
{
    asm volatile(
        "pusha\n"
        "pushw %es\n"
        "xor %ax, %ax\n"
        "mov %ax,%es"
    );
    asm volatile(
        "mov $0x1000, %di\n"
        "mov $0xE820, %eax\n"
        "xor %ebx, %ebx\n"
        "mov $24, %ecx\n"
        "mov $0x534D4150, %edx\n"
        "int $0x15\n"
    "loop:\n"
        "jc done\n"
        "cmp $0, %ebx\n"
        "jz done\n"
        "add %cx, %di\n"
        "mov $0xE820, %eax\n"
        "mov $24, %ecx\n"
        "int $0x15\n"
        "jmp loop"
    );

    asm volatile(
        "done:\n"
        "mov %%di, %%ax\n"
        "popw %%es\n"
        "popa" : "=a"(size)
    );
    size = size - 0x1000;
    size = size / 20;
    size++;
}

void unreal_mode()
{
    asm volatile(
        "pusha\n"
        "cli\n"
        "pushw %ds\n"
        "pushw %es\n"

    	// enable A20
    	"in $0x92, %al\n"
    	"or $2, %al\n"
    	"out %al, $0x92\n"

    	// switch to unreal mode
    	"lgdtw descriptor\n"
    	"mov %cr0, %eax\n"
    	"or $1, %al\n"
    	"mov %eax, %cr0\n"
        "jmp pmode\n"

    "pmode:\n"
        // finish switching
    	"mov $0x10, %bx\n"
    	"mov %bx, %ds\n"
    	"mov %bx, %es\n"
    	"and $0xfe, %al\n"
    	"mov %eax, %cr0\n"
        "jmp unrealmode\n"
    "unrealmode:\n"
    	"popw %es\n"
    	"popw %ds\n"
        "mov  $0, %al\n"
        "movl $3840, %ecx\n"
        "movl $0xb8000, %edi\n"
        "addr32 rep stosb\n"
        "popa"
    );
}

dap d;
int n;
uint32_t load_pos;

int iter;
void load()
{
    asm volatile("pusha");
    asm volatile("mov load_pos, %ebx");
    asm volatile("mov n, %dx");
    asm volatile(
        "xor %eax, %eax\n"
    "load_loop:\n"
        // while (eax < edx)
        "cmp %edx, %eax\n"
        "jge load_exit\n"
        "inc %eax\n"

        "push %eax\n"
        "push %edx\n"
        "push %ebx\n"
        "mov $0x42, %ah\n"
        "mov $d, %si\n"
        "mov boot_disk, %dl\n"
        "int $0x13\n"
        "pop %ebx\n"
        "pop %edx\n"
        "pop %eax\n"

        // data is loaded at 0x1000 to ebx
        "movl $0x6000, %esi\n"
        "movl %ebx, %edi\n"
        "xor %ecx, %ecx\n"
        "mov $512, %cx\n"
        "addr32 rep movsb\n"

        // add one to dap_lba_low (go to next sector)
        "incl d+8\n"
    	"add $512, %ebx\n"

        "jmp load_loop\n"
    "load_exit:\n"
    );
    asm volatile("popa");
}

void check_for_long_mode()
{
    asm volatile("pusha");
    uint32_t eax;
    asm volatile("cpuid": "=a"(eax) : "a"(0x80000000));
    if(eax < 0x80000001)
        panic("I need 64-bit cpu!");
    asm volatile("cpuid": "=d"(eax) : "a"(0x80000001));
    if(!(eax & (1 << 29)))
        panic("I need 64-bit cpu!");
    asm volatile("popa");
}

uint32_t jump_to;
uint16_t boot_disk;
void main()
{
    boot_disk = *((uint16_t*)0x1000);
    do_e820();
    unreal_mode();
    check_for_long_mode();

    // find place for our kernel
    mmap* map = (mmap*)0x1000;
    for(int i = 0; i < size; i++)
    {
        if(map->len >= 0x80 * 512 && map->type == 1 && map->base != 0 && (map->len + 0x80 * 512) < 0xFFFFFFFF)
        {
            n = 0x80;
            load_pos = map->base;
            load();
            asm volatile(
                "push $0x8\n"
                "push %0\n"
                "lret": : "r"(map->base));
        }
        map++;
    }
    panic("cannot find place for kernel!");
}

// when entering kernel
// 32 bit protected mode, no paging
// long mode must be allowed
// eax = location of memory map
// ebx = len of memory map
