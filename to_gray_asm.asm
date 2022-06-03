; Laborator 6, pagina 16
; Atenție la formatul diff: https://en.wikipedia.org/wiki/Diff#Unified_format

.code
public to_gray_asm
to_gray_asm proc SYSTEMV uses RBX RSI RDI,          ; Windows: fără SYSTEMV
       image:qword, im_width:qword, im_height:qword
    ; image: RCX (Windows) / RDI (Linux)
    ; width: RDX (Windows) / RSI (Linux)
    ; height: R8 (Windows) / RDX (Linux)

    ; ...
mov RAX, im_width
mul im_height
mov RSI, image
mov RDI, RAX
mov R10, RSI
add R10, RDI
mov R11, 0FEFEFEFEFEFEFEFEh
mov R9, 00101010101010101h

bucla:
    mov RAX, qword ptr [RSI] ; R
    mov RBX, qword ptr [RSI+RDI] ; G
    mov RCX, qword ptr [RSI+RDI*2] ; B
    add RAX, R11
    add RBX, R11
    add RCX, R11
    add RAX, RBX
    rcr RAX, 1
    add RBX, RCX
    rcr RBX, 1
    and RAX, R11
    and RBX, R11
    add RAX, RBX
    rcr RAX, 1
    add RAX, R9
    mov qword ptr [RSI], RAX
    mov qword ptr [RSI+RDI], RAX
    mov qword ptr [RSI+RDI*2], RAX
    add RSI, 8
    cmp RSI, R10
    jb bucla
    ret
to_gray_asm endp
end