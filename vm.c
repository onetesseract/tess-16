#include <bits/types.h>
#include <stdio.h>
#include <stdint.h>

uint16_t regs[16];

uint8_t memory[4096];

uint16_t get_word(FILE* file) {
    uint16_t ret;
    ret = (uint8_t) fgetc(file);
    ret = ret | (((uint16_t) 0x00FF & (uint8_t) fgetc(file)) << 8);
    return ret;
}

int main(int argc, char** argv) {
    int current = 642;
    if (argc < 2) { return 1; }
    printf("Opening %s\n", argv[1]);
    FILE* file = fopen(argv[1], "r");
    current = fgetc(file);
    while(current != EOF) {
        printf("Current: %x\n", current);
        regs[0] += 4 * 2; // increment PC by 4 words
        switch (current) {
            case 0x00 : { // MOVR
                uint16_t imm16_0 = get_word(file);
                uint16_t reg_0 = get_word(file);
                printf("MOVR: imm16_0: %#04x reg_0: %#04x\n", imm16_0, reg_0);
                memory[imm16_0] = regs[reg_0];
                break;
            }

            case 0x21 : { // LOADI
                uint16_t reg_0 = get_word(file);
                uint16_t imm16_0 = get_word(file);
                printf("LOADI: imm16_0: %#04x reg_0: %#04x\n", imm16_0, reg_0);
                regs[reg_0] = imm16_0;
                break;
            }

            case 0x31 : { // ADDI
                uint16_t reg_0 = get_word(file);
                uint16_t imm16_0 = get_word(file);
                printf("ADDI: imm16_0: %#04x reg_0: %#04x\n", imm16_0, reg_0); // todo: fix ordering
                regs[reg_0] = regs[reg_0] + imm16_0;
                printf("Result: %#04x", regs[reg_0]);
                break;
            }
        }
        fseek(file, regs[0], SEEK_SET);
        current = fgetc(file);
    }
}