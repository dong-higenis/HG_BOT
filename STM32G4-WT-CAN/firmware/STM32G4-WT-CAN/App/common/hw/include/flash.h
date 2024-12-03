#ifndef FLASH_H_
#define FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_FLASH


bool flashInit(void);
bool flashErase(uint32_t addr, uint32_t length);
bool flashWrite(uint32_t addr, uint32_t *p_data, uint32_t length);
bool flashCalibWrite(uint32_t addr, uint64_t * data, uint32_t length);
bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length);
uint64_t flashCalibRead(uint32_t addr);

#endif


#ifdef __cplusplus
}
#endif

#endif 
