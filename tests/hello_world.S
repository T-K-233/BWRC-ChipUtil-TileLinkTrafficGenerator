#define UART_BASE_ADDR            	0x54000000
#define UART_TXDATA_ADDR            (UART_BASE_ADDR)
#define UART_RXDATA_ADDR            (UART_BASE_ADDR + 0x04)
#define UART_TXCTRL_ADDR            (UART_BASE_ADDR + 0x08)
#define CLINT_MTIME                 0x0200BFF8

_start:
  j main
  
HAL_UART_init:
  # enable UART
  li t0, 1
  sw t0, 8(s0)
  
  ret

HAL_delay:
  lw t0, 0(s1)
  add t1, t0, a0
  
  wait_for_time:
  lw t0, 0(s1)
  bge t0, t1, end_delay
  j wait_for_time
  end_delay:
  ret


HAL_UART_transmit:
  
  wait_for_fifo:
  lw t0, 8(s0)
  srli t0, t0, 16
  bne t0, zero, wait_for_fifo
  
  sw a0, 0(s0)
  ret
  

main:
  li s0, UART_BASE_ADDR
  li s1, CLINT_MTIME

  call HAL_UART_init
  
  loop:
    li a0, 'H'
    call HAL_UART_transmit
	
    li a0, 'e'
    call HAL_UART_transmit
	
    li a0, 'l'
    call HAL_UART_transmit
	
    li a0, 'l'
    call HAL_UART_transmit
	
    li a0, 'o'
    call HAL_UART_transmit
	
    li a0, '\n'
    call HAL_UART_transmit
    
    li a0, 1000
    call HAL_delay
	
	j loop

