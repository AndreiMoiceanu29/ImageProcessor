.code
public convolution
to_gray_asm proc SYSTEMV uses RBX RSI RDI,  
    image:qword, im_width:qword, im_height:qword, kernel: qword, k_size: qword
    
    mov RAX, k_size
    mov RBX, 2
    div RBX ; k_size = k_size/2
    .for(r9=0 : r9 < im_height : r9++)
        .for(r10=0 : r10 < im_width : r10++)
            mov r11, 0; r
            mov r12, 0; g
            mov r13, 0; b
            .for(r14=-1 : r14 < rax : r14++)
                .for(r15=-1 : r15 < rax : r15++)
                    mov rcx, r9
                    sub rcx, r14 ; x = x - i
                    mov rdx, r10
                    sub rdx, r15; y = y - j
                    cmp rcx, 0
                    .if(EQUALS?)
                        .continue
                    .endif
                    cmp rdx, 0
                    .if(EQUALS?)
                        .continue
                    .endif
                    cmp rcx, im_height
                    .if(GREATER?)
                        .continue
                    .endif
                    cmp rdx, im_width
                    .if(GREATER?)
                        .continue
                    .endif
                    mov rsi, image
                    mov rdi, kernel
                    
                .endfor
            .endfor
        .endfor
    .endfor


convolution endp
end