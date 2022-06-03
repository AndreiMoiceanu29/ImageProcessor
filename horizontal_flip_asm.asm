.code
public _horizontal_flip_asm
_horizontal_flip_asm proc SYSTEMV uses RBX RSI RDI,          ; Windows: fără SYSTEMV
       image:qword, im_width:qword, im_height:qword

    mov RAX, im_width
    sar RAX, 1
    mov R9, RAX ; R9 = im_width / 2
    mov r10, 0
    mov r11, 0
    mov r13, im_height
    .for(r10=0 : r10 < r13 : r10++)
        .for(r11=0 : r11 < r9 : r11++)
        ; Compute the first address 4*width*r10 + 4*r11 + 0
        mov RAX, 4
        mov RBX, r13
        mul RBX
        mov RBX, r10
        mul RBX
        mov r12, RAX
        mov RAX, 4
        mov RBX, r11
        mul RBX
        add r12, RAX ; 4 * width * y + 4 * x + 0
        ;mov r13, r12 ; 4 * width * y + 4 * x + 1
        ;add r13, 1
       ; mov r14, r12 ; 4 * width * y + 4 * x + 2
       ; add r14, 2
        ; Compute the new address 4*width*r10 + 4 * (width - r11 - 1) + 0
        mov RAX, 4
        mov RBX, r13
        mul RBX
        mov RBX, r10
        mul RBX
        mov r8, RAX
        mov RAX, 4
        mov RBX, r13
        sub RBX, r11
        sub RBX, 1
        mul RBX
        add r8, RAX ; 4 * width * y + 4 * (width - x - 1) + 0
       ; mov r15, r8
       ; add r15, 1 ; 4 * width * y + 4 * (width - x - 1) + 1
       ; mov RCX, r8
       ; add RCX, 2 ; 4 * width * y + 4 * (width - x - 1) + 2
        ; Swap the bytes
        mov eax , [image+r12]
        mov ebx, [image+r8]
        mov [image + r12], eax
        mov [image + r8], ebx

        ; mov al, [image+r13]
        ; mov bl, [image + r15]
        ; mov [image+r13], bl
        ; mov [image+r15], al

        ; mov al, [image+r14]
        ; mov bl, [image + RCX]
        ; mov [image+r14], bl
        ; mov [image+RCX], al
        .endfor
        ; Compute the second address 4*width*r10 + 4*r11 + 1
        ; Compute the third address 4*width*r10 + 4*r11 + 2
    .endfor

ret

_horizontal_flip_asm endp
end