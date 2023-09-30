// cSpell:ignore laihost

#include "attributes.h"
#include <cstdlib>
#include <klog/klog.h>
#include <misc/kassert.h>
#include <mm/mm.h>
#include <mm/paging/paging.h>
#include <pci/pci.h>
#include <lai/include/lai/core.h>

C_ABI void laihost_log(int level, const char* msg) {}
C_ABI NORETURN void laihost_panic(const char* msg) { klog::panic(msg); }
C_ABI auto laihost_malloc(std::size_t size) -> void* { return new std::uint8_t[size]; }
C_ABI auto laihost_realloc(void* pointer, std::size_t size) -> void* { return std::realloc(pointer, size); }

void laihost_outb(std::uint16_t port, std::uint8_t val) { return outb(port, val); }
void laihost_outw(std::uint16_t port, std::uint16_t val) { return outw(port, val); }
void laihost_outd(std::uint16_t port, std::uint32_t val) { return outl(port, val); }

auto laihost_inb(std::uint16_t port) -> std::uint8_t { return inb(port); }
auto laihost_inw(std::uint16_t port) -> std::uint16_t { return inw(port); }
auto laihost_ind(std::uint16_t port) -> std::uint32_t { return inl(port); }

auto laihost_pci_readb(std::uint16_t seg, std::uint8_t bus, std::uint8_t slot, std::uint8_t fun, std::uint16_t offset) -> std::uint8_t
{
    expect(seg == 0, "bad segment");
    return pci::pci_device_ident(bus, slot).read_config_byte(fun, offset);
}
auto laihost_pci_readw(std::uint16_t seg, std::uint8_t bus, std::uint8_t slot, std::uint8_t fun, std::uint16_t offset) -> std::uint16_t
{
    expect(seg == 0, "bad segment");
    return pci::pci_device_ident(bus, slot).read_config_word(fun, offset);
}

auto laihost_pci_readd(std::uint16_t seg, std::uint8_t bus, std::uint8_t slot, std::uint8_t fun, std::uint16_t offset) -> std::uint32_t
{
    expect(seg == 0, "bad segment");
    return pci::pci_device_ident(bus, slot).read_config_long(fun, offset);
}

void laihost_pci_writeb(std::uint16_t seg, std::uint8_t bus, std::uint8_t slot, std::uint8_t fun, std::uint16_t offset, std::uint8_t val) 
{
    expect(seg == 0, "bad segment");
    pci::pci_device_ident(bus, slot).write_config_byte(fun, offset, val);
}


void laihost_pci_writew(std::uint16_t seg, std::uint8_t bus, std::uint8_t slot, std::uint8_t fun, std::uint16_t offset, std::uint16_t val) 
{
    expect(seg == 0, "bad segment");
    pci::pci_device_ident(bus, slot).write_config_word(fun, offset, val);
}

void laihost_pci_writel(std::uint16_t seg, std::uint8_t bus, std::uint8_t slot, std::uint8_t fun, std::uint16_t offset, std::uint32_t val) 
{
    expect(seg == 0, "bad segment");
    pci::pci_device_ident(bus, slot).write_config_long(fun, offset, val);
}

auto laihost_map(std::size_t address, std::size_t count) -> void *
{
    paging::map_hhdm_section(address, count, {.rw = true}, true); 
    return as_vptr(mm::make_virtual(address));
}

void laihost_unmap(void *pointer, std::size_t count)
{
    // no-op
    (void) pointer;
    (void) count;
}

void laihost_sleep(std::uint64_t ms)
{
    expect(false, "not implemented");
}

auto laihost_timer() -> std::uint64_t
{
    expect(false, "not implemented");
    __builtin_unreachable();
}

void laihost_handle_amldebug(lai_variable_t *var);

void laihost_handle_global_notify(lai_nsnode_t *node, int code);
