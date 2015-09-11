#include <stdint.h>

#define MCP9800_PCB_ADDRESS 0x90
#define MCP9800_IN1_ADDRESS 0x92
#define MCP9800_IN2_ADDRESS 0x96
#define MCP9800_EXT_ADDRESS 0x9E

void MCP_9800_init(void);
int16_t MCP9800_get_tmp(uint8_t address);
