fmt_str_noerr = """
.align 0x10
    push $0
    push ${0}
    jmp idt_common
"""

fmt_str_err = """
.align 0x10
    push ${0}
    jmp idt_common
"""

f = open("idt.S.in", "r")
f.seek(0)
s = f.read()
s += """

    // auto generated IDT handler stubs

"""

has_error = {8,10,11,12,13,14,17}

for n in range(0, 256):
    if n in has_error:
        s += fmt_str_err.format(n)
    else:
        s += fmt_str_noerr.format(n)

o = open("idt.S", "w")
o.write(s)
