#define UART_BASE_ADDR            	0x54000000
#define UART_TXDATA_ADDR            (UART_BASE_ADDR)
#define UART_RXDATA_ADDR            (UART_BASE_ADDR + 0x04)
#define UART_TXCTRL_ADDR            (UART_BASE_ADDR + 0x08)

li s0, UART_BASE_ADDR

# enable UART
li s1, 1
sw s1, 8(s0)

infinite_loop:
li t0, 0x68
sw t0, 0(s0)
li t0, 0x65
sw t0, 0(s0)
li t0, 0x6c
sw t0, 0(s0)
li t0, 0x6c
sw t0, 0(s0)
li t0, 0x6f
sw t0, 0(s0)
li t0, 0x20
sw t0, 0(s0)
li t0, 0x77
sw t0, 0(s0)
li t0, 0x6f
sw t0, 0(s0)
li t0, 0x72
sw t0, 0(s0)
li t0, 0x6c
sw t0, 0(s0)
li t0, 0x64
sw t0, 0(s0)
li t0, 0x20
sw t0, 0(s0)
li t0, 0x20
sw t0, 0(s0)
li t0, 0x2d
sw t0, 0(s0)
li t0, 0x2d
sw t0, 0(s0)
li t0, 0x4f
sw t0, 0(s0)
li t0, 0x53
sw t0, 0(s0)
li t0, 0x43
sw t0, 0(s0)
li t0, 0x49
sw t0, 0(s0)
li t0, 10
sw t0, 0(s0)
j infinite_loop
