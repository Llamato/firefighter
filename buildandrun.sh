acme -o assets/mysprites.prg mysprites.asm
mos-c64-clang -Os main.c gllm/gllm.c common.c -o main.prg
x64sc main.prg