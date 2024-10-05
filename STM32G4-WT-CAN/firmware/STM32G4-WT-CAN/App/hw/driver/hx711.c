#include "hx711.h"
#include "gpio.h"
#include "cli.h"

#ifdef _USE_HW_HX711


//#define   _hx711DelayUsLopp  4

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
    delay(1);
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
  hx711SetCoef(354.5f);
#ifdef _USE_HW_CLI
  cliAdd("hx711", cliHx711);
#endif
}

int32_t hx711Value()
{
  uint32_t data = 0;
  uint32_t  startTime = millis();
  while(gpioPinRead(HW_GPIO_CH_HX711_DAT))
  {
    delay(1);
    if(millis() - startTime > 150)
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

int32_t hx711ValueAve(uint16_t sample)
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
  hx711Unlock();
}

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
  float answer =  (data - hx711.offset) / hx711.coef;
  hx711Unlock();
  return answer;
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
    cliPrintf("coef %d.%d\n", hx711.coef, hx711.coef/10);
    cliPrintf("offset: %d\n", hx711.offset);
    cliPrintf("status: %s\n", hx711.status==0?"POWER OFF":"POWER ON");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    while(cliKeepLoop())
    {
      cliPrintf("%f\n", hx711Weight(10));
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

  if (args->argc == 2 && args->isStr(0, "calibration") == true)
  {
    int32_t raw = args->getData(1);
    int known_weight = args->getData(2);
    cliPrintf("calibration %dkg\n", known_weight);
    hx711Calibration(0, known_weight);
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
  }

  if (ret != true)
  {
    cliPrintf("hx711 info\n");
    cliPrintf("hx711 show\n");
    cliPrintf("hx711 power [on:off]\n");
    cliPrintf("hx711 write coef val\n");    
    cliPrintf("hx711 read coef\n");
    cliPrintf("hx711 calibration [kg]\n");
    //cliPrintf("hx711 cali\n");
    //cliPrintf("hx711 read cali\n");
  }
}
#endif


#endif
