;
; arch/ia32/start.asm
; Copyright (c) 2005 Luke McCarthy
;
; Entry point and CPU initialisation.
;
; NOTE: Don't use the .bss section. There seems to be a linkage problem
; with NASM object files that causes bss symbols to be set to 0 (causing
; no end of weird bugs!)
;

use32

global _start, trap_entry_tab, irq_entry_tab, syscall_entry_point, \
       page_fill, page_copy

extern main, idt, idt_init, tss_init, irq_entry, trap_entry, \
       _estack, locus_current, locus_put, idle_locus, cls, mbinf, tss, \
       syscall_entry, kern_drtab, kern_pgtab, lock_level, _eimg, _eintstack

%define TOPHYS(p) ((p)-0xFF000000)

%define SELECTOR(n) ((n)<<3)
%define KERN_CS SELECTOR(1)
%define KERN_DS SELECTOR(2)
%define USER_CS SELECTOR(3)
%define USER_DS SELECTOR(4)
%define KERN_TSS SELECTOR(5)

GRUB_MAGIC	equ 0x1BADB002
GRUB_ALIGN_4K	equ 1<<0
GRUB_MEMINFO	equ 1<<1
GRUB_FLAGS	equ GRUB_ALIGN_4K | GRUB_MEMINFO

%define LOCUS_ESP 0x00

section .rodata

	dd GRUB_MAGIC, GRUB_FLAGS, -(GRUB_MAGIC + GRUB_FLAGS)
	align 64, db 0
idtr:
	dw 49*8-1
	dd idt
	dw 0
gdt:
	dw gdt.end-gdt-1
	dd gdt
	dw 0
	dd 0x0000ffff, 0x00cf9a00  ; kernel code segment
	dd 0x0000ffff, 0x00cf9200  ; kernel data segment
	dd 0x0000ffff, 0x00cffa00  ; user code segment
	dd 0x0000ffff, 0x00cff200  ; user data segment
.tss:	dd 0x00000067, 0x00008900  ; tss for kernel stack
.end:

section .text

;; Entry point
_start:
	cld
	cli
;; Save multiboot information
	cmp eax, 0x2BADB002
	je .ismb
	xor ebx, ebx  ; NULL
.ismb:
	mov [TOPHYS(mbinf)], ebx
;; Initialise kernel page table
	mov eax, TOPHYS(_eimg) - 0x1000
	or eax, 0x103 ; present | writable | global
	mov ecx, TOPHYS(_eimg) - 0x1000
	shr ecx, 12  ; divide by page size
.pgt:
	mov dword [TOPHYS(kern_pgtab)+ecx*4], eax
	sub eax, 0x1000
	dec ecx
	jns .pgt
;; TODO: Clear rest of kernel address space
;; Initialise kernel page directory
	mov eax, TOPHYS(kern_pgtab)
	or eax, 0x003  ; present | writable
	mov dword [TOPHYS(kern_drtab)+0xFF0], eax
	mov dword [TOPHYS(kern_drtab)+0x000], eax
	add eax, 0x1000
	mov dword [TOPHYS(kern_drtab)+0xFF4], eax
	mov dword [TOPHYS(kern_drtab)+0x004], eax
	add eax, 0x1000
	mov dword [TOPHYS(kern_drtab)+0xFF8], eax
	mov dword [TOPHYS(kern_drtab)+0x008], eax
	add eax, 0x1000
	mov dword [TOPHYS(kern_drtab)+0xFFC], eax
	mov dword [TOPHYS(kern_drtab)+0x00C], eax
;; Load page directory
	mov eax, TOPHYS(kern_drtab)
	mov cr3, eax
;; Enable PSE (4 MiB pages)
;	mov eax, cr4
;	or eax, 1<<4
;	mov cr4, eax
;; Enable paging
	mov eax, cr0
	or eax, 1<<31
	mov cr0, eax
	jmp dword .flush
.flush:
;; Enable global pages (must be done AFTER paging is enabled)
	mov eax, cr4
	or eax, 1<<7
	mov cr4, eax
;; Load GDT
	lgdt [gdt]
	mov eax, cr0
	or al, 1
	mov cr0, eax
	nop
;; Load CS and EIP to kernel area
	jmp KERN_CS:.loadcs
.loadcs:
	mov ax, KERN_DS
	mov ss, ax
	mov ax, USER_DS | 3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov esp, _estack
;; Floppy motor off (GRUB leaves it on...)
	xor eax, eax
	mov dx, 0x3f2  ; TODO: check boot device is a floppy!
	out dx, al
;; Cursor off
	mov dx, 0x3d4  ; CRTC registers address port
	mov al, 0xa    ; 'Cursor Start' register index
	out dx, al     ; select it
	inc dx         ; CRTC registers data port
	in al, dx      ; fetch bits
	or al, 1<<5    ; 'Cursor Off' bit
	out dx, al     ; store bits
;; Initialise PIC
	mov al, 0x11
	out 0x20, al
	out 0xa0, al
	mov al, 32     ; IRQ 0-7 -> INT 32-39
	out 0x21, al
	mov al, 40     ; IRQ 8-15 -> INT 40-47
	out 0xa1, al
	mov al, 4
	out 0x21, al
	mov al, 2
	out 0xa1, al
	mov al, 1
	out 0x21, al
	out 0xa1, al
	mov al, 0xff
	out 0x21, al
	mov al, 0xfb
	out 0xa1, al
;; Unmask IRQs
	xor al, al
	out 0x21, al
	out 0xa1, al
;; Timer
	mov al, 0x34
	out 0x43, al
	mov eax, 0x2e9b
	out 0x40, al
	shr eax, 8
	out 0x40, al
;; Initialise TSS descriptor
	mov eax, tss
	and eax, 0xffffff
	or [gdt.tss+2], eax
	mov eax, tss
	and eax, 0xff000000
	or [gdt.tss+4], eax
;; Initialise TSS
	mov dword [tss+0x04], _eintstack  ; esp0
	mov dword [tss+0x08], KERN_DS     ; ss0
	mov dword [tss+0x66], 0x8000      ; io_bitmap_off
;; Load Task Register
	mov ax, KERN_TSS
	ltr ax
	inc dword [lock_level]
;; Load IDT
	call idt_init
	lidt [idtr]
;; Finish intialisation
	call main
	dec dword [lock_level]
;; Idle
	sti
.idle:
	hlt
	jmp .idle

;; Low-level interrupt handling

align 4
syscall_entry_point:
;	cld
;	push 48  ; dummy interrupt number
;	push 0   ; dummy error code
	pushad
;	push ds
;	mov ax, KERN_DS
;	mov ds, ax
;	mov es, ax
	call syscall_entry
;	test eax, eax
;	jnz switch
;	pop ds
;	mov ax, ds
;	mov es, ax
	popad
;	lea esp, [esp+8]
	iretd

align 4
trap_entry_tab:
	push byte 0
	jmp short handle_trap_noerr
	push byte 1
	jmp short handle_trap_noerr
	push byte 2
	jmp short handle_trap_noerr
	push byte 3
	jmp short handle_trap_noerr
	push byte 4
	jmp short handle_trap_noerr
	push byte 5
	jmp short handle_trap_noerr
	push byte 6
	jmp short handle_trap_noerr
	push byte 7
	jmp short handle_trap_noerr
	push byte 8
	jmp short handle_trap
	push byte 9
	jmp short handle_trap_noerr
	push byte 10
	jmp short handle_trap
	push byte 11
	jmp short handle_trap
	push byte 12
	jmp short handle_trap
	push byte 13
	jmp short handle_trap
	push byte 14
	jmp short handle_trap
	push byte 15
	jmp short handle_trap_noerr
	push byte 16
	jmp short handle_trap_noerr
	push byte 17
	jmp short handle_trap
	push byte 18
	jmp short handle_trap_noerr
	push byte 19
handle_trap_noerr:
	push dword [esp]      ; dup trap number
	mov dword [esp+4], 0  ; dummy error code
handle_trap:
	cli
	cld
	pushad
;	push ds
;	mov ax, KERN_DS
;	mov ds, ax
;	mov es, ax
	call trap_entry
	test eax, eax
	jnz switch
;	pop ds
;	mov ax, ds
;	mov es, ax
	popad
	lea esp, [esp+8]
	iretd

align 4
irq_entry_tab:
	push byte 0
	jmp short handle_irq_master
	push byte 1
	jmp short handle_irq_master
	push byte 2
	jmp short handle_irq_master
	push byte 3
	jmp short handle_irq_master
	push byte 4
	jmp short handle_irq_master
	push byte 5
	jmp short handle_irq_master
	push byte 6
	jmp short handle_irq_master
	push byte 7
	jmp short handle_irq_master
	push byte 8
	jmp short handle_irq_slave
	push byte 9
	jmp short handle_irq_slave
	push byte 10
	jmp short handle_irq_slave
	push byte 11
	jmp short handle_irq_slave
	push byte 12
	jmp short handle_irq_slave
	push byte 13
	jmp short handle_irq_slave
	push byte 14
	jmp short handle_irq_slave
	push byte 15
 	jmp short handle_irq_slave
handle_irq_master:
	cld
	push dword [esp]      ; dup irq number
	mov dword [esp+4], 0  ; dummy error code
	pushad
;	push ds
;	mov ax, KERN_DS
;	mov ds, ax
;	mov es, ax
	mov al, 0x20  ; tell PIC
	out 0x20, al
	push handle_irq_switch
	jmp irq_entry
handle_irq_slave:
	cld
	push dword [esp]      ; dup irq number
	mov dword [esp+4], 0  ; dummy error code
	pushad
;	push ds
;	mov ax, KERN_DS
;	mov ds, ax
;	mov es, ax
	mov al, 0x62  ; tell PIC
	out 0x20, al
	mov al, 0x20
	out 0xa0, al
;	out 0x20, al
	call irq_entry
handle_irq_switch:
	test eax, eax
	jnz switch
;	pop ds
;	mov ax, ds
;	mov es, ax
	popad
	lea esp, [esp+8]
	iretd

; some important offsets -- see include/ia32/locus.h

%define REGS_KSP 0x0C
%define REGS_EIP 0x20
%define REGS_CS  0x24
%define REGS_EFL 0x28
%define REGS_ESP 0x2C
%define REGS_SS  0x30
%define REGS_CR3 0x34

%define TRAP_PUSHAD 0x00  ;0x04
%define TRAP_EIP    0x28  ;0x2C
%define TRAP_CS     0x2C  ;0x30
%define TRAP_EFL    0x30  ;0x34
%define TRAP_ESP    0x34  ;0x38

;; Context switch
switch:
	mov ebx, [locus_current]
;	lea ecx, [esp+TRAP_PUSHAD]
;	mov [ebx+REGS_KSP], ecx
	mov [ebx+REGS_KSP], esp
	test dword [esp+TRAP_CS], 3  ; test for kernel thread
	jz .restore
.saveuser:
; save user general registers
	lea esi, [esp+TRAP_PUSHAD]
	mov edi, ebx
	mov ecx, 8
	cld
	rep movsd
; save user interrupt frame
	mov ecx, [esp+TRAP_EIP]
	mov [ebx+REGS_EIP], ecx
	mov ecx, [esp+TRAP_EFL]
	mov [ebx+REGS_EFL], ecx
	mov ecx, [esp+TRAP_ESP]
	mov [ebx+REGS_ESP], ecx
.restore:
	mov [locus_current], eax  ; set new current locus
	test dword [eax+REGS_CS], 3
	jz .reskern
	mov edx, [eax+REGS_CR3]
	mov ecx, cr3
	cmp ecx, edx  ; different address space?
	je .resuser
	mov cr3, edx
; restore user-mode thread
.resuser:
;	mov cx, USER_DS | 3
;	mov ds, cx
;	mov es, cx
;	mov fs, cx
;	mov gs, cx
	mov esp, eax  ; restore from register struct
	popad
	iretd
; restore kernel-mode thread
.reskern:
	mov esp, [eax+REGS_KSP]  ; load saved kernel ESP
	popad                    ; restore GPRs from stack
	lea esp, [esp+8]         ; skip interrupt number and error
	iretd

;; Memory operations

page_fill:
	mov edx, [esp+4]
	mov eax, [esp+8]
	mov ecx, 0x80
align 16
.loop:
	mov [edx+0x00], eax
	mov [edx+0x04], eax
	mov [edx+0x08], eax
	mov [edx+0x0c], eax
	mov [edx+0x10], eax
	mov [edx+0x14], eax
	mov [edx+0x18], eax
	mov [edx+0x1c], eax
	add edx, 0x20
	dec ecx
	jnz .loop
	ret

page_copy:
	push edi
	push esi
	push ebp
	push ebx
	mov edi, [esp+16+4]
	mov esi, [esp+16+8]
	mov ecx, 0x100
align 16
.loop:
	mov eax, [esi+0x0]
	mov ebx, [esi+0x4]
	mov edx, [esi+0x8]
	mov ebp, [esi+0xC]
	mov [edi+0x0], eax
	mov [edi+0x4], ebx
	mov [edi+0x8], edx
	mov [edi+0xC], ebp
	add esi, 0x10
	add edi, 0x10
	dec ecx
	jnz .loop
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret
