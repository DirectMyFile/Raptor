#pragma once

#include <cpuid.h>
#include <kernel/panic.h>
#include <liblox/io.h>
#include <stdint.h>

#include "cmdline.h"

#define cpu_equals(name) __builtin_cpu_is(name)
#define cpu_supports(feature) __builtin_cpu_supports(feature)
#define get_cpuid(in, a, b, c, d) __get_cpuid(in, a, b, c, d)

typedef struct regs {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} regs_t;

#define IRQ_CHAIN_SIZE 16
#define IRQ_CHAIN_DEPTH 4

typedef void (*irq_handler_t) (regs_t *);
typedef int (*irq_handler_chain_t) (regs_t *);

#define breakpoint(name) \
    if (cmdline_bool_flag("break-" name)) {\
      puts("[BREAK] " name "\n"); \
      panic(NULL); \
    }
