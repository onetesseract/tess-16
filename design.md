## tess-16

16 bit word width  
no memory mapping  
14 GP regs - 2 through F  
PC, SP regs accessible - 0 through 1  

# instructions  
all instructions will have a length of 4 words, last arg padded with 0s

0x00  | MOVR imm16-0 reg-0     => *imm16-0 = reg-0
0x01  | MOVI imm16-0 imm16-1   => *imm16-0 = imm16-1
0x02  | MOVP imm16-0 imm16-1   => *imm16-0 = *imm16-1
0x03  | MOVRP reg-0 imm16-0    => *reg-0 imm16-0

0x10  | STAI reg-0 imm16-0     => reg-0 = imm16
0x11  | STAP reg-0 imm16-0     => reg-0 = *imm16-0

0x20  | LOADR reg-0 reg-1      => reg-0 = reg-1
0x21  | LOADI reg-0 imm16-0    => reg-0 = imm16-0
0x22  | LOADP reg-0 imm16-0    => reg-0 = *imm16-0

0x30  | ADDR reg-0 reg-1       => reg-0 = reg-0 + reg-1
0x31  | ADDI reg-0 imm16-0     => reg-0 = reg-0 + imm16-0

0x40  | IFR reg-0 reg-1 reg-2  => if (reg-2 == 0) reg-0 = reg-1