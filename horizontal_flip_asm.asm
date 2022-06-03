.code
public horizontal_flip
horizontal_flip proc SYSTEMV uses RBX RSI RDI,          ; Windows: fără SYSTEMV
       image:qword, im_width:qword, im_height:qword

    mov RAX, im_width
    mov RBX, 2
    mov RDX, 0 ; clear
    div RAX
    mov R9, RAX ; R9 = im_width / 2
    .for(r10=0 : r10 < im_height : r10++)
        .for(r11=0 : r11 < r9 : r11++)
        ; Compute the first address 4*width*r10 + 4*r11 + 0
        mov RAX, 4
        mov RBX, im_width
        mul RBX
        mov RBX, r10
        mul RBX
        mov r12, RAX
        mov RAX, 4
        mov RBX, r11
        mul RBX
        add r12, RAX
        mov r13, r12
        add r13, 1
        mov r14, r12
        add r14, 2
        mov al , image[r12]
        ; Compute the new address 4*width*r10 + 4 * (width - r11 - 1) + 0
        mov RAX, 4
        mov RBX, im_width
        mul RBX
        mov RBX, r10
        mul RBX
        mov r8, RAX
        mov RAX, 4
        mov RBX, im_width
        sub RBX, r11
        sub RBX, 1
        mul RBX
        add r8, RAX
        mov r15, r8
        add r15, 1
        mov r16, r8
        add r16, 2
        mov image[r12], image[r8]
        mov image[r8], al
        mov al, image[r13]
        mov image[r13], image[r15]
        mov image[r15], al
        mov al, image[r14]
        mov image[r14], image[r16]
        mov image[r16], al
        .endfor
        ; Compute the second address 4*width*r10 + 4*r11 + 1
        ; Compute the third address 4*width*r10 + 4*r11 + 2
    .endfor



horizontal_flip endp
end