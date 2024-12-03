 #include "ap.h"

void updateCan(void);
void apCalibration(int32_t noload_raw, int32_t load_raw, float scale);
float hexToFloat(uint32_t hexValue);

const uint32_t can_cmd_list[] = {
  CAN_ID_COMMANDER_WT_CAL_SET,
};
#define MAX_CAN_CMD_LIST (sizeof(can_cmd_list)/sizeof(uint32_t))

uint8_t wt_state = 0;


typedef struct
{
  uint32_t      offset;
  float         coef;
} cali_data_t;

const uint32_t flashLastAddr =  0x801F800;


#ifdef _USE_HW_CLI
static void cliApp(cli_args_t *args);
#endif


void apInit (void)
{
  cliOpen(HW_UART_CH_DEBUG, 115200);
  logBoot(true);
  canOpen(_DEF_CAN1, CAN_NORMAL, CAN_CLASSIC, CAN_500K, CAN_500K);

  uint64_t raw_data = flashCalibRead(flashLastAddr);
  cali_data_t data = *((cali_data_t *)&raw_data);

  if(isnan(data.coef))
  {
    apCalibration(8765020, 12524616, 5);
  }
  else
  {
    hx711SetOffset(data.offset);
    hx711SetCoef(data.coef);
  }


#ifdef _USE_HW_CLI
  cliAdd("app", cliApp);
#endif
}

void apMain (void)
{
  uint32_t pre_time;
  uint32_t led_case = 0;

  pre_time = millis ();
  while (1)
  {
    if (millis () - pre_time >= 500)
    {
      led_case++;
      if(led_case >= 3){
        led_case = 0;
      }
      pre_time = millis ();
      ledToggle (_DEF_LED1);
    }

    updateCan();
    cliMain();
  }
}


void updateCan(void)
{
  static uint8_t data_count = 0;
  static uint32_t pre_time = 0;
  can_msg_t msg;
  hx711UpdateWeight();
  if(millis() - pre_time > 100)
  {
    pre_time = millis();
//    int16_t weight =  (int16_t)(((hx711GetLastWeight()*100.0f)+0.5f)/10.0f); // 1.0(1kg) to 10
    int16_t weight =  (int16_t)(hx711GetLastWeight() * 1000); // 1.0(1kg) to 1000
    int32_t value = hx711GetLastValue();

    data_count++;

    msg.frame   = CAN_CLASSIC;
    msg.id_type = CAN_EXT;
    msg.dlc     = CAN_DLC_8;
    msg.id      = CAN_ID_WT_WEIGHT;
    msg.length  = CAN_DLC_8;
    msg.data[0] = data_count;
    msg.data[1] = wt_state;
    msg.data[2] = (weight >> 8);
    msg.data[3] = (weight & 0xFF);

    msg.data[4] = (value >> 24);
    msg.data[5] = (value >> 16);
    msg.data[6] = (value >> 8);
    msg.data[7] = (value & 0xFF);

    if (canMsgWrite(_DEF_CAN1, &msg, 10) > 0)
    {

    }
  }
  if (canMsgAvailable(_DEF_CAN1))
  {
    canMsgRead(_DEF_CAN1, &msg);
    for(uint8_t i=0;i<MAX_CAN_CMD_LIST;i++)
    {
      if(msg.id == can_cmd_list[i])
      {
        cliPrintf("0x%08X, L:%02d, ", msg.id, msg.length);
        for (int i=0; i<msg.length; i++)
        {
          cliPrintf("0x%02X ", msg.data[i]);
        }
        cliPrintf("\n");

        switch(msg.id)
        {
        case CAN_ID_COMMANDER_WT_CAL_SET:
          if(msg.data[0] == 0x01)
          {
            cliPrintf("[%02X] Measure the cur weight and obtain raw data\n", msg.data[0]);
            wt_state = 1;
          }
          if(msg.data[0] == 0x02)
          {
            cliPrintf("[%02X] Measure the cur weight, obtain raw data, and calibrate to %fKG.\n",
                      msg.data[0], (float)((msg.data[1] << 16 | msg.data[2])/10.0f));
            wt_state = 2;
          }
          break;
        }
      }
    }
  }
}

void updateCalibration(void)
{
  static uint32_t pre_time = 0;
  int32_t value = 0;
  int32_t noload_value_avg = 0;
  uint8_t noload_value_avg_count = 0;

  if(wt_state == 1)
  {
    if(millis() - pre_time > 100)
    {
      pre_time = millis();
        value = hx711GetLastValue();
      noload_value_avg += value;
      noload_value_avg_count++;
      if(noload_value_avg_count >= 10)
      {
        noload_value_avg /= 10;
        cliPrintf("Raw Value: \n", noload_value_avg);
        noload_value_avg_count = 0;
        wt_state = 0;
      }
    }
  }
}

void apCalibration(int32_t noload_raw, int32_t load_raw, float scale)
{
  hx711Calibration(noload_raw, load_raw, scale);
  int32_t offset = hx711GetOffset();
  float coef = hx711GetCoef();

  cali_data_t cali_data = {offset, coef};

  flashErase(flashLastAddr, sizeof(cali_data_t));
  flashCalibWrite(flashLastAddr, (uint64_t *)&cali_data, sizeof(cali_data_t));
}


#ifdef _USE_HW_CLI
void cliApp(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 4 && args->isStr(0, "cali") == true)
  {
    int32_t  no_load_raw = args->getData(1);
    int32_t  load_raw = args->getData(2);
    int32_t  load_weight = args->getData(3);
    cliPrintf("calibration:\n");
    cliPrintf("no laod raw value: %d\n", no_load_raw);
    cliPrintf("load raw value: %d\n", load_raw);
    cliPrintf("weight: %dKG\n", load_weight);
    apCalibration(no_load_raw, load_raw, load_weight);
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf("app cali [no raw] [load raw] [kg]\n");
  }
}
#endif

