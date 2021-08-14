CC=gcc
CPPC=g++
CPPFLAGS=-fno-rtti -fno-exceptions -lc -c -march=i386 -Os -ffreestanding -m32 -fno-PIC -fno-exceptions -fcf-protection=none
LD=gcc
CFLAGS=-c -march=i386 -Os -m16 -ffreestanding -fno-PIC
LDFLAGS=-Wl,-m -Wl,elf_i386 -static -nostdlib -Wl,--nmagic 

kernel.img : build/boot.bin build/kernel.bin
	dd if=build/boot.bin of=$@
	cat build/kernel.bin >> $@
	truncate -s 10321920 $@

build/boot.o : bootloader/bootloader.S build/
	$(CC) $(CFLAGS) -m16 $< -o $@

build/boot.bin : build/boot.o bootloader/bootloader.lds build/
	$(LD) $(LDFLAGS) -Tbootloader/bootloader.lds -o $@ build/boot.o

build/kernel/%.o : kernel/%.cpp build/kernel/
	$(CPPC) $(CPPFLAGS) $< -o $@

build/kernel/lib/%.o : kernel/lib/%.cpp build/kernel/lib/
	$(CPPC) $(CPPFLAGS) $< -o $@
	
build/kernel.bin : build/kernel/kernel.o build/kernel/idt.o build/kernel/lib/memory.o build/kernel/lib/printf.o build/kernel/lib/putc.o
	$(LD) $(LDFLAGS) -Tkernel/kernel.lds -o $@ build/kernel/kernel.o build/kernel/idt.o build/kernel/lib/memory.o build/kernel/lib/printf.o build/kernel/lib/putc.o -lgcc

build/ffmpegout/ : bad_apple.mp4 build/
	mkdir -p build/ffmpegout
	ffmpeg -i bad_apple.mp4 -r 1/1 $$build/ffmpegout/frame%05d.bmp

build/kernel/lib/ : build/kernel/
	mkdir build/kernel/lib/ -p
build/kernel/ : build/
	mkdir build/kernel -p
build/ : 
	mkdir build -p
