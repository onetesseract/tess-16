#include <bits/types.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // slight pain, just a bit

#ifdef VERBOSE
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...) /* do nothing */
#endif

void port0(uint16_t data);
void port1(uint16_t data);

typedef void (port_call_t)(uint16_t);

uint16_t regs[16];

uint64_t true_pc = 0;

uint8_t memory[4096];

port_call_t* ports[] = {port0, port1};

typedef enum variable_type {
    reg = 0,
    deref_reg,
    imm16,
    deref_imm16 = 3,
} variable_t;

char* variable_type_names[4] = {
    "reg",
    "deref_reg",
    "imm16",
    "deref_imm16",
};

void print_vartype(variable_t variable_type) {
    if(variable_type > 3) { printf("invalid"); }
    else printf("%s", variable_type_names[variable_type]);
}

uint16_t get_word(FILE* file) {
    uint16_t ret;
    uint16_t tmp;
    ret = (uint8_t) memory[true_pc];
    true_pc++;
    ret = ret | (((uint16_t) 0x00FF & (uint8_t) memory[true_pc]) << 8);
    true_pc++;
    return ret;
}

void port0(uint16_t data) { // this just halts the program until user presses enter
    #ifdef VERBOSE
    printf("port0: %#04x\n", data);
    #else
    printf("%#04x\n", data);
    #endif
    getchar();
}

void port1(uint16_t data) { // this prints to stdout as ASCII
    char ascii = (char) data;
    #ifdef VERBOSE
    printf("port1: %c - %#04x\n", ascii, data);
    #else
    printf("%c", ascii);
    #endif

}

void parse_variables(variable_t output[3], uint8_t byte) {
    output[0] = (byte & 0b11000000) >> 6;
    output[1] = (byte & 0b00110000) >> 4; // no i do not know how to binary logic
    output[2] = (byte & 0b00001100) >> 2;
    // output[3] = (byte & 0b11000011) >> 0;
}

void set(variable_t variable_type, uint16_t index, uint16_t val) {
    #ifdef VERY_VERBOSE
    printf("setting variable-type: ");
    print_vartype(variable_type);
    printf(" with index %d\n", index, val);
    #endif
    switch(variable_type) {
        case reg: {
            regs[index] = val;
            break;
        }
        case deref_reg: {
            memory[regs[index]] = val;
            break;
        }
        case imm16: {
            // whot
            printf("Tried to set?? an imm16???\n");
            exit(1);
            break;
        }
        case deref_imm16: {
            memory[index] = val;
            break;
        }
    }
}

uint16_t get(variable_t variable_type, uint16_t val) {
    uint16_t ret;
    #ifdef VERY_VERBOSE
    printf("getting variable-type: ");
    print_vartype(variable_type);
    printf(" with index %d\n", val);
    #endif
    switch(variable_type) {
        case reg: {
            ret = regs[val];
            break;
        }
        case deref_reg: {
            debug("Getting addr %d\n", regs[val]);
            ret = memory[regs[val]];
            break;
        }
        case imm16: {
            ret = val;
            break;
        }
        case deref_imm16: {
            ret = memory[val];
            break;
        }
    }
    debug("Got value %d\n", ret);
    return ret;
}

int main(int argc, char** argv) {
    int current;
    int suffix;
    int file_len;
    int count = 1;
    int c = 'e';
    int file_count = 0;
    variable_t variables[3];
    regs[0] = 0;
    if (argc < 2) { return 1; }
    printf("Opening %s\n", argv[1]);
    FILE* file = fopen(argv[1], "r");
    fseek(file, 0, SEEK_END);
    file_len = ftell(file);
    fseek(file, 0, SEEK_SET);
    c = fgetc(file);
    while(c != EOF) {
        memory[file_count] = (uint8_t) c;
        c = fgetc(file);
        file_count++;
    }
    printf("Read %d bytes\n", file_count);
    current = memory[0];
    suffix = memory[1];
    true_pc = 2;
    while(regs[0] <= file_len - 1) {
        #ifdef VERBOSE
        printf("Current: %x\n", current);
        printf("Memory index: %d\n", true_pc);
        printf("Suffix: %x\n", suffix);
        printf("Count: %d\n", count);
        #endif
        parse_variables(variables, suffix);
        #ifdef VERY_VERBOSE
        for(int i = 0; i < 4; i++) {
            printf("Variable %d: ", i);
            print_vartype(variables[i]);
            printf("\n");
        }
        #endif
        regs[0] += 4 * 2; // increment PC by 4 words
        switch (current) {
            case 0x00 : { // HLT
                exit(0);
                break;
            }
            case 0x01 : { // MOV
                uint16_t output = get_word(file);
                uint16_t input = get_word(file);
                #ifdef VERBOSE
                printf("MOV: output: %#04x input: %#04x\n", output, input);
                #endif
                set(variables[0], output, get(variables[1], input));
                #ifdef VERBOSE
                printf("Result: %#04x\n", get(variables[0], output));
                #endif
                break;
            }

            case 0x02 : { // ADD
                uint16_t output = get_word(file);
                uint16_t inp1 = get_word(file);
                uint16_t inp2 = get_word(file);
                #ifdef VERBOSE
                printf("ADD: output: %#04x inp1: %#04x inp1: %#04x\n", output, inp1, inp2); // todo: fix ordering
                #endif
                set(variables[0], output, (get(variables[1], inp1) + get(variables[2], inp2)));
                #ifdef VERBOSE
                printf("Result: %#04x\n", get(variables[0], output));
                #endif
                break;
            }

            case 0x03 : { // SUB
                uint16_t output = get_word(file);
                uint16_t input1 = get_word(file);
                uint16_t input2 = get_word(file);
                #ifdef VERBOSE
                printf("SUB: output: %#04x inp1: %#04x inp1: %#04x\n", output, input1, input2);
                #endif
                set(variables[0], output, (get(variables[1], input1) - get(variables[2], input2)));
                #ifdef VERBOSE
                printf("Result: %#04x\n", get(variables[0], output));
                #endif
                break;
            }

            case 0x04 : { // IFNZ
                uint16_t output = get_word(file);
                uint16_t input = get_word(file);
                uint16_t check = get_word(file);
                #ifdef VERBOSE
                printf("IFNZ: output: %#04x inp1: %#04x check: %#04x\n", output, input, check);
                #endif
                if(get(variables[2], check) != 0) {
                    set(variables[0], output, get(variables[1], input));
                }
                #ifdef VERBOSE
                printf("Result: %#04x\n", get(variables[0], output));
                #endif
                break;
            }

            case 0x05 : { // out
                uint16_t port = get_word(file);
                uint16_t data = get_word(file);
                #ifdef VERBOSE
                printf("OUT: port: %#04x data: %04x\n", port, data);
                #endif
                data = get(variables[1], data);
                (*ports[port])(data);
                break;
            }
        }
        true_pc = regs[0], SEEK_SET;
        current = memory[true_pc];
        suffix = memory[true_pc + 1];
        true_pc += 2;
        count++;
    }
}