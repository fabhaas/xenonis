; ---------- (C) 2018, 2019 fahaas ----------

section .text
global XENONIS_add
  
; to call from C, add: extern uint64_t add(uint64_t *a, uint64_t *b, uint64_t *c, size_t size)
; for C++: extern "C" std::uint64_t add(std::uint64_t *a, std::uint64_t *b, std::uint64_t *c, std::size_t size)
; arguments with Linux x86_64 calling conventions
; a - rdi
; b - rsi
; c - rdx
; size - rcx (size % 4 == 0 MUST BE TRUE)
XENONIS_add:
    push r12
    push r13
    ; push r14
    ; push r15
    xor r8, r8      ; r8 = i = 0
    mov r9, 4       ; r9 = 4
    mov rax, rcx    ; rax = size
    mov rcx, rdx    ; rcx = c
    xor rdx, rdx    ; rdx = 0
    div r9          ; rdx is remainder, rax = size
.mloop:
    mov r10, QWORD [rdi+r8*8]
    mov r11, [rdi+r8*8+8]
    mov r12, QWORD [rdi+r8*8+16]
    mov r13, QWORD [rdi+r8*8+24]
    ; mov r14, QWORD [rdi+32+r8*8]
    ; mov r15, QWORD [rdi+40+r8*8]
    adcx r10, QWORD [rsi+r8*8]
    adcx r11, QWORD [rsi+8+r8*8]
    adcx r12, QWORD [rsi+16+r8*8]
    adcx r13, QWORD [rsi+24+r8*8]
    ; adcx r12, QWORD [rsi+32+r8*8]
    ; adcx r13, QWORD [rsi+40+r8*8]
    mov QWORD [rcx+r8*8], r10
    mov QWORD [rcx+8+r8*8], r11
    mov QWORD [rcx+16+r8*8], r12
    mov QWORD [rcx+24+r8*8], r13
    ; mov QWORD [rcx+32+r8*8], r14
    ; mov QWORD [rcx+40+r8*8], r15
    adox r8, r9
    dec rax
    jnz .mloop
    
    inc rdx ; do not change the state of the CF
    dec rdx
    jz .end
.rloop:
    mov r10, QWORD [rdi+r8*8]
    adc r10, QWORD [rsi+r8*8]
    mov QWORD [rcx+r8*8], r10
    inc r8
    dec rdx
    jnz .rloop
.end:
    mov rax, 0 ; must use mov instead of xor, because of CF
    adc rax, 0
    ; pop r15
    ; pop r14
    pop r13
    pop r12
    ret
