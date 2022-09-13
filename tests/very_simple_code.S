li t0, 0x80001000
li t1, 0xDEADBEEF
sw t1, 0(t0)
infinite_loop:
j infinite_loop


