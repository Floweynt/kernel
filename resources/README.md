# build-resources

A bunch of ugly binary blobs and a few shell scripts.

- `gen-dbg` generates some symbols used for debugging. This is added as a kernel module
- `generate_image.sh` generates the disk image for qemu booting
- `git_version_info.sh` subs in git branch/hash info into the config
- `kernel-template.img` is the template image used to generate the rest of the kernel
- `OVMF.fd` an UEFI
- `strip_debug.sh` removes debugging information from the stripped binary
