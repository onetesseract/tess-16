import sys

opcodes = {
    "mov": 0x00,
    "add": 0x01,
}

suffix_codes = {
    "r": 0,
    "d": 1,
    "i": 2,
    "p": 3,
}

output = []
input_file = ""

def read_line():
    global input_file
    out = ""
    c = ""
    while c != "\n" and len(input_file) > 0:
        c = input_file[0]
        out += c
        input_file = input_file[1:]
    
    return out

def compile_suffix(suffix_str):
    val = 0
    count = 4
    for i in suffix_str:
        if count == 0: break
        code = suffix_codes[i.lower()]
        code = code << int((count * 2) - 2)
        val = val | code
        count -= 1
    return val


def compile_line(line):
    print(f"Line: {line}")
    split = line.strip().split(" ")
    instruction_kw, suffix = split[0].split(".")[0].lower(), split[0].split(".")[1].lower()
    opcode = opcodes[instruction_kw]
    suffix_code = compile_suffix(suffix)
    compiled = [opcode, suffix_code]
    for i in split[1:]:
        if i == "": continue
        i = int(i)
        high = i & 0xFF00
        low = i & 0x00FF
        compiled.append(low)
        compiled.append(high)
    if len(compiled) > 8:
        raise Exception("Too much info!")
    while len(compiled) != 8:
        compiled.append(0)
    print(f"Compiled: {compiled}")
    return bytearray(compiled)


if __name__ == "__main__":
    input_file = open(sys.argv[-1], "r").read()
    c = "a"
    ret = bytearray()
    while c != "":
        c = read_line()
        if c.strip() == "": continue
        out = compile_line(c)
        print(f"Out: {out}")
        ret += out
    print(ret)
    output_file = open(sys.argv[-1] + ".out-sendhelp", "wb")
    output_file.write(ret)
    