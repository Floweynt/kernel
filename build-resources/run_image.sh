#!/bin/bash
qemu-system-x86_64 -bios /usr/share/ovmf/x64/OVMF.fd -hda $1 -no-reboot -monitor stdio -d int -D qemu.log -no-shutdown &
#gdb $2 -ex 'target remote localhost:1234' -ex 'b _start' -ex 'c' -ex 'tui enable'
