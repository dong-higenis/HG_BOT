#include "bsp.h"
#include "hw_def.h"


extern TIM_HandleTypeDef htim4;

bool bspInit(void)
{
  HAL_TIM_Base_Start(&htim4);

  return true;
}

void delay(uint32_t ms)
{
#ifdef _USE_HW_RTOS
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
    osDelay(ms);
  }
  else
  {
    HAL_Delay(ms);
  }
#else
  HAL_Delay(ms);
#endif
}

uint32_t millis(void)
{
  return HAL_GetTick();
}

