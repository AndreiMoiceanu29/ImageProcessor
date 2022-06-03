# pentru C/ASM: make c_asm
c_asm: image_processor.c lodepng.c lodepng.h to_gray_asm.asm
	uasm247_osx/uasm -f macho -q to_gray_asm.asm
	gcc -O2 image_processor.c lodepng.c to_gray_asm.o -o to_gray -ggdb3

# pentru C fără ASM: make c
c: image_processor.c lodepng.c lodepng.h
	gcc -O2 image_processor.c lodepng.c -o to_gray -ggdb3

# pentru C++/ASM: make cpp_asm
# fișierele sursă trebuie redenumite din *.c în *.cpp
cpp_asm: image_processor.cpp lodepng.cpp lodepng.h to_gray_asm.asm
	uasm -elf64 to_gray_asm.asm
	g++ -O2 image_processor.cpp lodepng.cpp to_gray_asm.o -o to_gray -ggdb3 -fpermissive

# pentru C++ fără ASM: make cpp
# fișierele sursă trebuie redenumite din *.c în *.cpp
cpp: image_processor.cpp lodepng.cpp lodepng.h
	g++ -O2 image_processor.cpp lodepng.cpp -o to_gray -ggdb3 -fpermissive

clean:
	rm -f to_gray *.o *.s
