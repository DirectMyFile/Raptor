#pragma once

// File defining common x86 structures.

typedef struct {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} regs_t;

#define IRQ_CHAIN_SIZE 16
#define IRQ_CHAIN_DEPTH 4

typedef void (*irq_handler_t) (struct regs *);
typedef int (*irq_handler_chain_t) (struct regs *);

