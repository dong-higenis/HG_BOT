#ifndef HX711_H_
#define HX711_H_


/*
  Author:     Nima Askari
  WebSite:    http://www.github.com/NimaLTD
  Instagram:  http://instagram.com/github.NimaLTD
  Youtube:    https://www.youtube.com/channel/UCUhY7qY1klJm1d2kulr9ckw
  
  Version:    1.1.1
  
  
  Reversion History:
  
  (1.1.1):
    Add power down/up.
  (1.1.0):
    Add structure, Add calibration, Add weight, change names, ...
  (1.0.0):
    First Release.
*/

#ifdef __cplusplus
extern "C" {
#endif
  
#include "hw_def.h"

#ifdef _USE_HW_HX711

typedef struct
{
  uint8_t       status;
  int32_t       offset;
  float         coef;
  int32_t       last_raw_data;
  float         last_weight;
  uint8_t       lock;    
} hx711_t;


void        hx711Init();
int32_t     hx711Value();
int32_t     hx711ValueAvg(uint16_t sample);
int32_t     hx711GetLastValue();
void        hx711SetCoef(float coef);
float       hx711GetCoef();
void        hx711Calibration(int32_t value_noload, int32_t value_load, float scale);
void        hx711Tare(uint16_t sample);
float       hx711Weight(uint16_t sample);
float       hx711UpdateWeight();
float       hx711GetLastWeight();
void        hx711PowerDown();
void        hx711PowerUp();

#ifdef __cplusplus
}
#endif

#endif 

#endif