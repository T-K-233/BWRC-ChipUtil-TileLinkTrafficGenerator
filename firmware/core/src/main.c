
#include "main.h"

volatile int done_status = 0;
char str[512];


// Command functions
// Load and send <bytes> bytes of data from address <addr>
void ble_send(uint32_t addr, uint32_t bytes) {
  reg_write32(BASEBAND_ADDITIONAL_DATA, addr);
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_SEND, 0, bytes));
}

// Configure baseband constant <target> (from secondary instruction set) to value <value>
void ble_configure(uint8_t target, uint32_t value) {
  reg_write32(BASEBAND_ADDITIONAL_DATA, value);
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_CONFIG, target, 0));
}

// Try and receive data. Any found data will be stored at address <addr>
void ble_receive(uint32_t addr) {
  reg_write32(BASEBAND_ADDITIONAL_DATA, addr);
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_RECEIVE, 0, 0));
}

// Try and receive data. Any found data will be stored at address <addr>
void ble_receive_exit() {
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_RECEIVE_EXIT, 0, 0));
}

uint32_t baseband_status0() {
  return reg_read32(BASEBAND_STATUS0);
}

uint32_t baseband_status1() {
  return reg_read32(BASEBAND_STATUS1);
}

uint32_t baseband_status2() {
  return reg_read32(BASEBAND_STATUS2);
}

uint32_t baseband_status3() {
  return reg_read32(BASEBAND_STATUS3);
}

uint32_t baseband_status4() {
  return reg_read32(BASEBAND_STATUS4);
}

uint32_t baseband_rxerror_message() {
  return reg_read32(BASEBAND_RXERROR_MESSAGE);
}

uint32_t baseband_txerror_message() {
  return reg_read32(BASEBAND_TXERROR_MESSAGE);
}

uint32_t baseband_rxfinish_message() {
  return reg_read32(BASEBAND_RXFINISH_MESSAGE);
}

// LUT Control
#define LUT_LOFSK 0
#define LUT_LOCT 1
#define LUT_AGCI 2
#define LUT_AGCQ 3
#define LUT_DCOIFRONT 4
#define LUT_DCOQFRONT 5

#define LUT_COMMAND(lut, address, value) ((lut & 0xF) + ((address & 0x3F) << 4) + ((value & 0x3FFFFF) << 10))

void baseband_set_lut(uint8_t lut, uint8_t address, uint32_t value) {
  reg_write32(BASEBAND_LUT_CMD, LUT_COMMAND(lut, address, value));
}

// Function written for writing to tuning MMIO, but technically can be used for all MMIO although unchecked
// For partial write (4 bits), offset is used to shift data to correct position
void baseband_tuning_set(uint32_t addr, uint32_t data, uint32_t bit_size, uint32_t offset) {
  // For partial writes, need to read, mask, write-back
  if (bit_size == 4) {
    uint8_t temp = (reg_read8(addr) & ~(15 << offset)) | (data << offset);
    // Debug print
    // printf("%x\n", temp);
    reg_write8(addr, temp);
  } else if (bit_size <= 8) {
    reg_write8(addr, data);
  } else {
    reg_write16(addr, data);
  }
}

// Function that tests (send + check) the baseband debug command
void baseband_debug(uint32_t addr, size_t byte_size) {
  // Sending baseband DEBUG instruction
  reg_write32(BASEBAND_ADDITIONAL_DATA, addr);
  
  // INST = data, 1111 1111 
  reg_write32(BASEBAND_INST, BASEBAND_INSTRUCTION(BASEBAND_DEBUG, -1, byte_size));

  // TODO: Determine cycles to wait. Printing for now
  sprintf(str, "Sent DEBUG instruction with data at address %.8x. Waiting...\n", addr);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  // Word-aligned address (word is 4 bytes, 32 bits)
  int mismatch = -1;
  int fail = 0;
  uint32_t res_addr = addr + ((byte_size & ~3) + ((byte_size % 4) > 0 ? 4 : 0)); 
  sprintf(str, "Output bytes at address %.8x: ", res_addr);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  for (int i = 0; i < byte_size; i++) {
    sprintf(str, "%.2x ", (unsigned)*(unsigned char*)(res_addr + i));
    HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    // Checking match
    if ((unsigned)*(unsigned char*)(addr + i) != (unsigned)*(unsigned char*)(res_addr + i)) {
      fail += 1;
      if (mismatch == -1) mismatch = i;
    }
  }
  if (fail) {
    sprintf(str, "FAILED TEST. %d bytes are mismatched in output. Index of first mismatch: %d\n", fail, mismatch);
  } else {
    sprintf(str, "PASSED TEST. All input bytes match output bytes.\n");
  }
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
}



typedef struct plic_context_control
{
  uint32_t priority_threshold;
  uint32_t claim_complete;
} plic_context_control_t __attribute__ ((aligned (0x1000)));


uint32_t *const plic_enables      =               (uint32_t* const) 0xc002000; // Context [0..15871], sources bitmap registers [0..31]
uint32_t *const plic_priorities   =               (uint32_t* const) 0xc000000; // priorities [0..1023]
plic_context_control_t *const plic_ctx_controls = (plic_context_control_t* const) 0xc200000; // Priority threshold & claim / complete for context [0..15871]

void plic_set_bit(uint32_t* const target, uint32_t index)
{
  uint32_t reg_index = index >> 5;
  uint32_t bit_index = index & 0x1F;
  *target |= (1 << bit_index);
}

void plic_enable_for_hart(uint32_t hart_id, uint32_t irq_id) {
  uint32_t* base = plic_enables + 32 * hart_id;
  plic_set_bit(base, irq_id);
}

void plic_set_priority(uint32_t irq_id, uint32_t priority) {
  plic_priorities[irq_id] = priority;
}

uint32_t plic_claim_irq(uint32_t hart_id) {
  return plic_ctx_controls[hart_id].claim_complete;
}

void plic_complete_irq(uint32_t hart_id, uint32_t irq_id){
  plic_ctx_controls[hart_id].claim_complete = irq_id;
}


int main() {
  HAL_init();

  UART_InitTypeDef UART_init_config;
  UART_init_config.baudrate = 10000;




  HAL_UART_init(UART0, &UART_init_config);
  HAL_GPIO_init(GPIOA, GPIO_PIN_0);

  // HAL_delay(2000);

  // set tuning trim G0 0th bit 1
  CLEAR_BITS(*(uint8_t *)BASEBAND_TRIM_G0, 0b1);




  sprintf(str, "TRIM G0 value: %x\n", *(uint8_t *)BASEBAND_TRIM_G0);
  HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

  // Payload is <header><data>, where data is "TEST DATA FOR BASEBAND!"
  uint8_t payload[]  = {0x1, 0x17, 0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54, 0x41, 0x20, 0x46, 0x4f, 0x52, 0x20, 0x42, 0x41, 0x53, 0x45, 0x42, 0x41, 0x4e, 0x44, 0x21};

  // Interrupts for baseband TX are 9, 10
  // plic_enable_for_hart(0, 9);
  // plic_enable_for_hart(0, 10);
  // plic_set_priority(9, 5); // Set all the priorities to 5 for now
  // plic_set_priority(10, 5);
  
  // HAL_CORE_enableIRQ(MachineExternal_IRQn);
  // HAL_CORE_enableInterrupt();


  while (1) {


  // uint32_t dcsr = 0;
  // asm volatile("csrr %0, dcsr" : "=r"(dcsr));

  // sprintf(str, "dcsr value: %x\n", dcsr);
  // HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);


    // sprintf(str, "Sending payload to baseband...\n");
    // HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);
    ble_send((uint32_t) payload, sizeof(payload));
    // while (done_status != 1) {
    //   sprintf(str, "*");
    // }
    // sprintf(str, "\nDone.\n");
    // HAL_UART_transmit(UART0, (uint8_t *)str, strlen(str), 0);

    // HAL_delay(100);
  }
}
