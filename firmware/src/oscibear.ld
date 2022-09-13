ENTRY(main)

MEMORY {
  SRAM(rwx): ORIGIN = 0x80000000, LENGTH = 8K
}

SECTIONS {
  .text : {
    *(.text)
  }> SRAM
}
