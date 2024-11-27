#include "encoder.h"

#ifdef _USE_HW_ENCODER
#include "../../common/hw/include/gpio.h"
#include "cli.h"


typedef struct
{
  GpioPinName_t gpio_a;
  GpioPinName_t gpio_b;

  int32_t  count;
  int32_t  dir;
} encoder_tbl_t;

typedef struct
{
  uint32_t count_mask;
  int32_t  count;
} encoder_eep_t;

#ifdef _USE_HW_CLI
static void cliEncoder(cli_args_t *args);
#endif
static void encoderISR(uint8_t ch, uint8_t edge);
static bool encoderInitGpio(void);


static encoder_tbl_t encoder_tbl[ENCODER_MAX_CH] =
{
  {R_ENC1, R_ENC2, 0, 1}, // gpio_a, gpio_b, count, dir
  {L_ENC1, L_ENC2, 0, 1},
};

static bool is_init = false;


bool encoderInit(void)
{
  bool ret = true;

  for (int i=0; i<ENCODER_MAX_CH; i++)
  {
    encoder_tbl[i].count = 0;
  }

  ret = encoderInitGpio();

  is_init = ret;


#ifdef _USE_HW_CLI
  cliAdd("encoder", cliEncoder);
#endif

  return true;
}


bool encoderInitGpio(void)
{
  return true;
}

bool encoderReset(void)
{
  for (int i=0; i<ENCODER_MAX_CH; i++)
  {
    encoder_tbl[i].count = 0;
  }
  return true;
}

bool encoderClearCount(uint8_t ch)
{
  if (ch >= ENCODER_MAX_CH) return false;

  encoder_tbl[ch].count = 0;

  return true;
}

int32_t encoderGetCount(uint8_t ch)
{
  int32_t count;

  if (ch >= ENCODER_MAX_CH)
    return 0;

  count = encoder_tbl[ch].count * encoder_tbl[ch].dir;

  return count;
}

bool encoderSetCount(uint8_t ch, int32_t count)
{
  if (ch >= ENCODER_MAX_CH)
    return 0;

  encoder_tbl[ch].count = count * encoder_tbl[ch].dir;

  return true;
}


void encoderISR(uint8_t ch, uint8_t edge)
{
  encoder_tbl_t *p_enc = &encoder_tbl[ch];
  uint8_t pin_a;
  uint8_t pin_b;

  pin_a = gpioPinRead(p_enc->gpio_a);
  pin_b = gpioPinRead(p_enc->gpio_b);

  if (edge == _DEF_CH_A){
      if (pin_a == _DEF_HIGH)
        {
          if (pin_b == _DEF_LOW){
              p_enc->count--;
          }
          else{
              p_enc->count++;
          }
        }
        else
        {
          if (pin_b == _DEF_HIGH){
              p_enc->count--;
          }
          else{
              p_enc->count++;
          }
        }
  } else {
      if (pin_a == _DEF_HIGH)
        {
          if (pin_b == _DEF_LOW){
              p_enc->count++;
          }
          else{
              p_enc->count--;
          }
        }
        else
        {
          if (pin_b == _DEF_HIGH){
              p_enc->count++;
          }
          else{
              p_enc->count--;
          }
        }
  }

  return;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == L_ENC1_Pin) // GPIO_PIN_4(PA4)
  {
    encoderISR(_DEF_CH2, _DEF_CH_A);
  }
  if (GPIO_Pin == R_ENC1_Pin) // GPIO_PIN_8(PA8)
  {
    encoderISR(_DEF_CH1, _DEF_CH_A);
  }

  if (GPIO_Pin == L_ENC2_Pin) // GPIO_PIN_5(PA5)
  {
    encoderISR(_DEF_CH2, _DEF_CH_B);
  }
  if (GPIO_Pin == R_ENC2_Pin) // GPIO_PIN_15(PA15)
  {
    encoderISR(_DEF_CH1, _DEF_CH_B);
  }
}

#ifdef _USE_HW_CLI
void cliEncoder(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info"))
  {
    while(cliKeepLoop())
    {
      for (int i=0; i<ENCODER_MAX_CH; i++)
      {
        cliPrintf("%8d ", encoderGetCount(i));
      }
      cliPrintf("\n");
      delay(50);
      cliMoveUp(1);
    }
    cliMoveDown(1);
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("encoder info\n");
  }
}
#endif

#endif
