#include "hx711.h"
#include "gpio.h"
#include "cli.h"

#ifdef _USE_HW_HX711
/* 
https://github.com/nimaltd/HX711 참고
*/

//#define   _hx711DelayUsLopp  4
#define HX711_READ_SAMPLE      10

static hx711_t hx711;


#ifdef _USE_HW_CLI
static void cliHx711(cli_args_t *args);
#endif

void hx711DelayUs(void)
{
  delayUs(1);
#if 0  
  uint32_t delay = _hx711DelayUsLopp;
  while (delay > 0)
  {
    delay--;
    __NOP(); __NOP(); __NOP(); __NOP();    
  }
#endif  
}

void hx711Lock()
{
  while (hx711.lock)
  {
    delay(1);
  }
  hx711.lock = 1;      
}

void hx711Unlock()
{
  hx711.lock = 0;
}

void hx711Init()
{  
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, true);  
  delay(10);
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, false);
  delay(10);  

  hx711Value();
  hx711Value();
  hx711Unlock(); 

#ifdef _USE_HW_CLI
  cliAdd("hx711", cliHx711);
#endif
}

int32_t hx711ValueNoDelay()
{
  uint32_t data = 0;
  if(gpioPinRead(HW_GPIO_CH_HX711_DAT))
  {
    return 0;
  }
  for(int8_t i=0; i<24 ; i++)
  {
    gpioPinWrite(HW_GPIO_CH_HX711_SCK, true);      
    hx711DelayUs();    
    gpioPinWrite(HW_GPIO_CH_HX711_SCK, false);  
    hx711DelayUs();
    data = data << 1;    
    if(gpioPinRead(HW_GPIO_CH_HX711_DAT))
      data ++;
  }
  data = data ^ 0x800000; 
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, true);   
  hx711DelayUs();
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, false);  
  hx711DelayUs();
  return data;    
}

int32_t hx711Value()
{
  uint32_t data = 0;
  uint32_t  startTime = millis();
  // 이 부분 수정 필요 지연이 될 수 있음
  while(gpioPinRead(HW_GPIO_CH_HX711_DAT))
  {
    delay(1);
    if(millis() - startTime > 150)
    {
      //not ready to read
      return 0;
    }
  }
  for(int8_t i=0; i<24 ; i++)
  {
    gpioPinWrite(HW_GPIO_CH_HX711_SCK, true);      
    hx711DelayUs();    
    gpioPinWrite(HW_GPIO_CH_HX711_SCK, false);  
    hx711DelayUs();
    data = data << 1;    
    if(gpioPinRead(HW_GPIO_CH_HX711_DAT))
      data ++;
  }
  data = data ^ 0x800000; 
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, true);   
  hx711DelayUs();
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, false);  
  hx711DelayUs();
  return data;    
}

int32_t hx711ValueAvg(uint16_t sample)
{
  hx711Lock();
  int64_t  ave = 0;
  for(uint16_t i=0 ; i<sample ; i++)
  {
    ave += hx711Value();
    delay(5);
  }
  int32_t answer = (int32_t)(ave / sample);
  hx711Unlock();
  return answer;
}

/*
초기 무게 값 측정해 offset 적용하기 위함
*/
void hx711Tare(uint16_t sample)
{
  hx711Lock();
  int64_t  ave = 0;
  for(uint16_t i=0 ; i<sample ; i++)
  {
    ave += hx711Value();
    delay(5);
  }
  hx711.offset = (int32_t)(ave / sample);
  // hx711ValueAvg(uint16_t sample) 함수 호출해서 offset에 담아도 되지 않나?
  hx711Unlock();
}

/*
noload_raw: 아무것도 없을때 무게 값
load_raw: 켈리브레이션용 물건의 무게 RAW 값
scale: 켈리브레이션용 물건의 무게 값(KG)
*/
void hx711Calibration(int32_t noload_raw, int32_t load_raw, float scale)
{
  hx711Lock();
  hx711.offset = noload_raw;
  hx711.coef = (load_raw - noload_raw) / scale;  
  hx711Unlock();
}

float hx711Weight(uint16_t sample)
{
  hx711Lock();
  int64_t  ave = 0;
  for(uint16_t i=0 ; i<sample ; i++)
  {
    ave += hx711Value();
    delay(5);
  }
  int32_t data = (int32_t)(ave / sample);
  hx711.last_raw_data = data;
  float answer =  (data - hx711.offset) / hx711.coef;
  hx711Unlock();
  return answer;
}

float hx711UpdateWeight()
{
  uint16_t sample = 10;
  static uint32_t tm = 0;
  static int64_t  avg = 0;
  static uint8_t count = 0;
  hx711Lock();
  if(millis() - tm >= 5)
  {
    tm = millis();
    int32_t val = hx711ValueNoDelay();
    if(val != 0)
    {
      avg += val;
      count++;
      if(count >= sample)
      {
        int32_t data = (int32_t)(avg / sample);
        hx711.last_raw_data = data;
        hx711.last_weight =  (data - hx711.offset) / hx711.coef;
        count = 0;
        avg = 0;
      }    
    }
  }
  hx711Unlock();
  return hx711.last_weight;
}

float hx711GetLastWeight()
{
  return hx711.last_weight;
}

int32_t hx711GetLastValue()
{
  return hx711.last_raw_data;
}

void hx711SetOffset(float offset)
{
  hx711.offset = offset;  
}

float hx711GetOffset()
{
  return hx711.offset;  
}

void hx711SetCoef(float coef)
{
  hx711.coef = coef;  
}

float hx711GetCoef()
{
  return hx711.coef;  
}

void hx711PowerDown()
{
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, false);    
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, true);    
  delay(1);  
  hx711.status = 0;
}

void hx711PowerUp()
{
  gpioPinWrite(HW_GPIO_CH_HX711_SCK, false);
  hx711.status = 1;
}


#ifdef _USE_HW_CLI
static void cliHx711(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf("hx711 \n");
//    cliPrintf("coef %d.%d\n", hx711.coef, hx711.coef/10);
    cliPrintf("coef %.1f\n", hx711.coef);
    cliPrintf("offset: %d\n", hx711.offset);
    cliPrintf("status: %s\n", hx711.status==0?"POWER OFF":"POWER ON");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    while(cliKeepLoop())
    {
      cliPrintf("%f(%d)\n", hx711Weight(HX711_READ_SAMPLE), hx711ValueAvg(HX711_READ_SAMPLE));
      delay(100);
    }
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "power") == true)
  {
    if(args->isStr(1, "on") == true)
    {
      hx711PowerUp();
    }
    else if(args->isStr(1, "off") == true)
    {
      hx711PowerDown();
    }
    ret = true;
  }

  if (args->argc == 4 && args->isStr(0, "cali") == true)
  {    
    int32_t  no_load_raw = args->getData(1);
    int32_t  load_raw = args->getData(2);
    int32_t  load_weight = args->getData(3);
    cliPrintf("calibration:\n");
    cliPrintf("no laod raw value: %d\n", no_load_raw);
    cliPrintf("laod raw value: %d\n", load_raw);
    cliPrintf("weight: %dKG\n", load_weight);
    hx711Calibration(no_load_raw, load_raw, load_weight);
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write") == true)
  {
    if(args->isStr(1, "coef") == true)
    {
      float coef = args->getFloat(2);
      cliPrintf("coef %f\n", hx711.coef);
      hx711SetCoef(coef);
      ret = true;
    }
  }
  if (args->argc == 2 && args->isStr(0, "read") == true)
  {
    if(args->isStr(1, "coef") == true)
    {
      cliPrintf("coef %f\n", hx711.coef);
      ret = true;
    }
    if(args->isStr(1, "raw") == true)
    {
      cliPrintf("raw %d\n", hx711ValueAvg(HX711_READ_SAMPLE));
      ret = true;
    }
  }
  if (args->argc == 3)
  {
    if(args->isStr(0, "read") == true && args->isStr(1, "raw") == true)
    {      
      uint16_t sampling = args->getData(2);
      cliPrintf("sampling: %d, raw %d\n", sampling, hx711ValueAvg(sampling));
      ret = true;
    }
  }

  if (ret != true)
  {
    cliPrintf("hx711 info\n");
    cliPrintf("hx711 show\n");
    cliPrintf("hx711 power [on:off]\n");
    cliPrintf("hx711 write coef val\n");    
    cliPrintf("hx711 read coef\n");
    cliPrintf("hx711 read raw sampling[1-1000]\n");
    cliPrintf("hx711 cali [no raw] [load raw] [kg]\n");
    //cliPrintf("hx711 cali\n");
    //cliPrintf("hx711 read cali\n");
  }
}
#endif


#endif
