# generate_image.sh <$1 out> <$2 kernel-template> <$3 elf> <$4 sym-module>

cp $2 "tmp.partition"
mcopy -i "tmp.partition" $3 "::kernel.elf"
mcopy -i "tmp.partition" $4 "::kernel_syms.kmod"
truncate -s 64M $1
parted -s $1 mklabel gpt
parted -s $1 mkpart ESP fat32 2048s 100%
parted -s $1 set 1 esp on
dd if="tmp.partition" of="$1" bs=1048576 seek=1 conv=notrunc 2>/dev/null
rm tmp.partition
