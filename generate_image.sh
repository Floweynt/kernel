touch kernel.img
truncate -s 64M kernel.img
parted -s image.img mklabel gpt
parted -s image.img mkpart ESP fat32 2048s 100%
parted -s image.img set 1 esp on
limine-install image.img
USED_LOOPBACK=$(sudo losetup -Pf --show image.hdd)
sudo mkfs.fat -F 32 ${USED_LOOPBACK}p1
mkdir -p img_mount
sudo mount ${USED_LOOPBACK}p1 img_mount
sudo mkdir -p img_mount/EFI/BOOT
sudo cp -v kernel.elf limine.cfg img_mount/
sudo cp -v /usr/share/limine/BOOTX64.EFI img_mount/EFI/BOOT/
sync
sudo umount img_mount
sudo losetup -d ${USED_LOOPBACK}
