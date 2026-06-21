acme -o assets/mysprites.prg mysprites.asm
mos-c64-clang -O2 main.c glibs64c/common.c glibs64c/gllm/gllm.c glibs64c/graphics.c glibs64c/hardware/*.c -Iglibs64c -Iglibs64c/gllm -Iglibs64c/hardware -o main.prg
#-Wl,-T,c64_protected.ld