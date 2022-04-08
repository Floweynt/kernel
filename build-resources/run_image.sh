#!/bin/bash
qemu-system-x86_64 -bios /usr/share/ovmf/x64/OVMF.fd -hda $1 -s -S &
gdb $2 -ex 'target remote localhost:1234' -ex 'b _start' -ex 'c'
