#include <halt.h>
#include <console.h>
#include <ia32/io.h>

#define read_pci(addr) (out32(0xCF8, (addr)), in32(0xCFC))

void init_devices()
{
	uint32 addr, info;

	for (addr = 0x80000000; addr < 0x81000000; addr += 0x800) {

		info = read_pci(addr);
		if (info == 0xFFFFFFFF)
			continue;

		print("* PCI device ");
		print_uint_right(info >> 16, 0x10, '0', 4);
		print(" vendor ");
		print_uint_right(info & 0xFFFF, 0x10, '0', 4);
		print(" at ");
		print_uint_right((addr >> 16) & 0xFF, 0x10, '0', 2);
		putchar(':');
		print_uint_right((addr >> 11) & 0x1F, 0x10, '0', 2);
		putchar('.');
		print_hex((addr >> 8) & 0x7); nl();

#if 0
		print_ptr(info);
		print(" Device/Vendor Number "); print_ptr(addr); nl();
		print_ptr(read_pci(addr + 0x10)); sp();
		print_ptr(read_pci(addr + 0x14)); sp();
		print_ptr(read_pci(addr + 0x18)); sp();
		print_ptr(read_pci(addr + 0x1C)); sp();
		print_ptr(read_pci(addr + 0x20)); sp();
		print_ptr(read_pci(addr + 0x24)); print(" Base Addresses\n");
		print_ptr(read_pci(addr + 0x30));
		print(" ROM Address\n\n");
#endif
  }
}
