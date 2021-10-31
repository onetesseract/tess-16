## tess-16

16 bit word width  
no mapping things to memory
14 GP regs - 2 through F  
PC, SP regs accessible - 0 through 1 but no use for SP yet - there is now, but it has to be hardcoded  

# instructions  
all instructions will have a length of 4 words, last arg padded with 0s  

Instruction format:  
byte opcode, byte suffix, word for each param, ending padded with 0s  
  
The suffix for each instruction defines what is being operated on, R for reg, I for imm16, P for *imm16 and D for *reg.  
Given that there are 64 possible combinations I will not list them all here and instead say represent them as a 4 long collection of 2 bit numbers, where the high bits are the first variable going down. 00 is R, 01 is D, 10 is I and 11 is P. (First bit is 0 for reg 1 for imm16, second bit is 0 for immediate or 1 for memory addr)

``` 
0x00  | halt => quits lol
 
0x01  | MOV.IR imm16-0 reg-0   => *imm16-0 = reg-0  
....  | MOV.II imm16-0 imm16-1 => *imm16-0 = imm16-1  
....  | MOV.PP imm16-0 imm16-1 => *imm16-0 = *imm16-1  
....  | MOV.DP reg-0 imm16-0   => *reg-0 imm16-0  
etc etc  

0x02  | ADD.RRR reg-0 reg-1 reg-2      => reg-0 = reg-1 + reg-2  
....  | ADD.RII reg-0 imm16-0 imm16-1    => reg-0 = imm16-0 + imm16-1  
etc etc  

0x03  | SUB.rrr reg-0 reg-1 reg-2      => reg-0 = reg-1 - reg-2  this is useless, will not be implemented and will be replaced when I figure out something to replace it with
  
0x04  | IFNZ reg-0 reg-1 reg-2  => if (reg-2 != 0) reg-0 = reg-1  

0x05  | OUT count val => this is special, the first variable type param is ignored
                         it will be used for inter-component sending but for now will call little internal
                         C routines.

0x06  | NOT output val => output = ~ val

0x07  | AND output val1 val2 => output = val1 & val2

etc etc
```

# known bugs  
for the program in test.tasm the first out after returning prints wrongly