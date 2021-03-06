#include <liblox/common.h>
#include <liblox/string.h>
#include <liblox/io.h>
#include <liblox/lox-internal.h>

#include <kernel/entry.h>
#include <kernel/tty/tty.h>
#include <kernel/cmdline.h>
#include <kernel/timer.h>
#include <kernel/device/driver.h>

#include "cpuid.h"
#include "cmdline.h"
#include "gdt.h"
#include "paging.h"
#include "keyboard.h"
#include "idt.h"
#include "debug.h"
#include "irq.h"
#include "pci_init.h"
#include "io.h"
#include "vga.h"

#include "devices/serial/serial.h"
#include "devices/pcnet/pcnet.h"
#include "devices/e1000/e1000.h"
#include "devices/vgafb/vgafb.h"
#include "devices/ata/ata.h"

void lox_output_char_ebl(char c) {
    outb(0x3F8, (uint8_t) c);
}

void lox_output_string_ebl(char* msg) {
    if (msg == NULL) {
        return;
    }

    size_t len = strlen(msg);
    for (size_t i = 0; i < len; ++i) {
        lox_output_char_ebl(msg[i]);
    }
}

void lox_output_string_vga(char* msg) {
    if (msg == NULL) {
        return;
    }
    vga_write_string(msg);
    lox_output_string_ebl(msg);
}

void lox_output_char_vga(char c) {
    vga_putchar(c);
    lox_output_char_ebl(c);
}

used void panic(nullable char* msg) {
    asm("cli;");

    if (msg != NULL) {
        lox_output_char_vga('\n');
        lox_output_string_vga("[PANIC] ");
        lox_output_string_vga(msg);
        lox_output_char_vga('\n');
    }

    while (true) {
        asm("hlt;");
    }
}

void (*lox_output_string_provider)(char*) = lox_output_string_ebl;
void (*lox_output_char_provider)(char) = lox_output_char_ebl;

syscall_result_t lox_syscall(syscall_id_t id, uintptr_t* args) {
    unused(id);
    unused(args);
    return 0;
}

syscall_result_t (*lox_syscall_provider)(
    syscall_id_t,
    uintptr_t*
) = lox_syscall;

void vga_pty_write(tty_t* tty, const uint8_t* bytes, size_t size) {
    unused(tty);

    vga_write((const char*) bytes, size);
}

void paging_init(void) {
    paging_install(mboot->mem_upper + mboot->mem_lower);

    if (mboot->flags & (1 << 6)) {
        mboot_memmap_t* mmap = (void*) mboot->mmap_addr;
        while ((uintptr_t) mmap < mboot->mmap_addr + mboot->mmap_length) {
            if (mmap->type == 2) {
                /* I don't know why we should use this type, but references use it. */
                for (unsigned long long int i = 0; i < mmap->length; i += 0x1000) {
                    if (mmap->base_addr + i > 0xFFFFFFFF) {
                        break;
                    }
                    uint32_t addr = (uint32_t) (mmap->base_addr + i);
                    paging_mark_system(addr & 0xFFFFF000);
                }
            }

            mmap = (mboot_memmap_t*) (
                (uintptr_t) mmap + mmap->size + sizeof(uintptr_t));
        }
    }

    paging_finalize();
}

extern void _init(void);

void kernel_setup_devices(void) {
    pci_init();

    vga_pty = tty_create("vga");
    vga_pty->ops.write = vga_pty_write;
    vga_pty->flags.allow_debug_console = true;
    vga_pty->flags.write_kernel_log = true;
    vga_pty->flags.echo = true;
    keyboard_init();
    tty_register(vga_pty);

    tty_serial_t* serial_port_a = tty_create_serial("serial-a", 0);
    serial_port_a->tty->flags.echo = true;
    serial_port_a->tty->flags.allow_debug_console = true;
    serial_port_a->tty->flags.write_kernel_log = true;
    tty_register(serial_port_a->tty);

    /**
     * Calls functions marked as a constructor in the C runtime.
     */
    _init();

    driver_register(ata_driver_setup);
    driver_register(pcnet_driver_setup);
    driver_register(e1000_driver_setup);
    driver_register(vgafb_setup);

    debug_x86_init();
}

/* Initial kernel stack pointer. */
used uintptr_t initial_esp = 0;

void kexec(const void* kernel, size_t size) {
    unused(kernel);
    unused(size);
}

used void kernel_main(multiboot_t* _mboot, uint32_t mboot_hdr, uintptr_t esp) {
    initial_esp = esp;

    if (mboot_hdr != MULTIBOOT_EAX_MAGIC) {
        return;
    }

    mboot = _mboot;
    init_cmdline(mboot);

    vga_init();

    if (!cmdline_bool_flag("disable-vga")) {
        lox_output_string_provider = lox_output_string_vga;
        lox_output_char_provider = lox_output_char_vga;
    }

    uint processor_type = cpuid_get_processor_type();
    if (processor_type == PROCESSOR_TYPE_INTEL) {
        puts(INFO "Processor Type: Intel\n");
    } else if (processor_type == PROCESSOR_TYPE_AMD) {
        puts(INFO "Processor Type: AMD\n");
    } else {
        puts(INFO "Processor Type: Unknown\n");
    }

    gdt_init();
    puts(DEBUG "GDT initialized.\n");
    idt_init();
    puts(DEBUG "IDT initialized.\n");
    isr_init();
    puts(DEBUG "ISRs initialized.\n");
    irq_init();
    puts(DEBUG "IRQs initialized.\n");
    timer_init(1000);
    puts(DEBUG "PIT initialized.\n");

    kernel_init();
}
