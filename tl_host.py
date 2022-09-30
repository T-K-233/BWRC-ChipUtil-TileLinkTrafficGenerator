import struct
import time

import serial

DEBUG_CONTROLLER_BASE   =0x00000000
BOOT_SELECT_BASE        =0x00002000
ERROR_DEVICE_BASE       =0x00003000
BOOTROM_BASE            =0x00010000
TILE_RESET_CTRL_BASE    =0x00100000
CLINT_BASE              =0x02000000
PLIC_BASE               =0x0C000000
LBWIF_RAM_BASE          =0x10000000
GPIO_BASE               =0x10012000
QSPI_BASE               =0x10040000
FLASH_BASE              =0x20000000
UART_BASE               =0x54000000
DTIM_BASE               =0x80000000



TL_CHANID_CH_A = 0
TL_CHANID_CH_D = 3
TL_OPCODE_A_PUTFULLDATA = 0
TL_OPCODE_A_GET = 4
TL_OPCODE_D_ACCESSACK = 0
TL_OPCODE_D_ACCESSACKDATA = 1



# prog_hex = [hex(w) for w in prog_int]

# print(prog_hex)

ser = serial.Serial("COM22", baudrate=2000000)


def TL_Get(addr, verbal=True):
    buffer = struct.pack("<BBBBLL", TL_CHANID_CH_A, TL_OPCODE_A_GET, 2, 0b11111111, addr, 0x00)
    ser.write(buffer)
    if verbal:
        print("[TL Get] <address: {0:08X}, size: {1}>".format(addr, 4))

    buffer = ser.read(12)
    chanid, opcode, size, denied, addr, data = struct.unpack("<BBBBLL", buffer)
    if opcode == TL_OPCODE_D_ACCESSACKDATA:
        if verbal:
            print("[TL AccessAckData] <size: {0}, data: 0x{1:08X}, denied: {2}>".format(4, data, denied))
        return data
    print("<ERROR!>")
    return -1


def TL_PutFullData(addr, data, verbal=True):
    buffer = struct.pack("<BBBBLL", TL_CHANID_CH_A, TL_OPCODE_A_PUTFULLDATA, 2, 0b11111111, addr, data)
    ser.write(buffer)
    if verbal:
        print("[TL PutFullData] <address: 0x{0:08X}, size: {1}, data: 0x{2:08X}>".format(addr, 4, data))

    buffer = ser.read(12)
    chanid, opcode, size, denied, addr, data = struct.unpack("<BBBBLL", buffer)
    if opcode == TL_OPCODE_D_ACCESSACK:
        if verbal:
            print("[TL AccessAck]".format())
        return
    print("<ERROR!>")
    return -1


def flash_prog():
    
    with open(r"C:\Users\TK\Desktop\BWRC-ChipUtil-TileLinkTrafficGenerator\firmware\build\firmware.bin", "rb") as f:
        prog_bin = f.read()

    size = len(prog_bin)
    n_words = size // 4

    print("program_size:", size, "bytes")

    prog_int = struct.unpack("<"+"L"*n_words, prog_bin)

    for addr, inst in enumerate(prog_int):
        print("{:.2f}%\t {} / {}".format((addr / n_words) * 100., addr, n_words))
        TL_PutFullData(0x80000000+addr*4, inst, verbal=False)

    time.sleep(0.1)

    TL_PutFullData(CLINT_BASE, 1)
    

def memory_scan():
    for addr in range(0x80005000, 0x80100000, 4):
        
        if not addr % 0x100:
            print("ADDRESS:", hex(addr))
        
        TL_PutFullData(addr, 0xFFFFFFFF, verbal=False)
        assert TL_Get(addr, verbal=False) == 0xFFFFFFFF
        TL_PutFullData(addr, 0x00000000, verbal=False)
        assert TL_Get(addr, verbal=False) == 0x00000000

def getUARTregs():
    for addr in range(UART_BASE, UART_BASE+0x1C, 4):
        TL_Get(addr)

def trigSoftwareInterrupt():
    TL_Get(CLINT_BASE)
    TL_PutFullData(CLINT_BASE, 1)
    time.sleep(1)
    TL_Get(CLINT_BASE)
    
def main():
    flash_prog()
    #time.sleep(0.02)
    #trigSoftwareInterrupt()

    #time.sleep(3)

    #trigSoftwareInterrupt()


    #while True:
    #    TL_Get(0x80005000)
    #    time.sleep(1)
    
    #memory_scan()
    #getUARTregs()

    #TL_Get(0x80000000)



main()
