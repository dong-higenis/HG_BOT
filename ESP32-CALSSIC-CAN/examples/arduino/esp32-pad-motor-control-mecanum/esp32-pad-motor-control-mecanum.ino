#include <Bluepad32.h>
#include "mecanum_driver.h"

uint8_t led_pin = 2;
uint8_t btn_pin = 27;
bool is_connected = false;
bool is_key_update = false;

robot::esp_motor_pin_t espMotorPins[] = {
    {16, 17, 0, 1}, // Front Left
    {14, 15, 6, 7}, // Front Right
    {21, 22, 2, 3}, // Back Left
    {23, 32, 4, 5}, // Back Right
};

robot::wheels wheel = robot::wheels(espMotorPins);

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl)
{
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
  {
    if (myControllers[i] == nullptr)
    {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      is_connected = true;
      break;
    }
  }
  if (!foundEmptySlot)
  {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl)
{
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
  {
    if (myControllers[i] == ctl)
    {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      is_connected = false;
      break;
    }
  }

  if (!foundController)
  {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

void dumpGamepad(ControllerPtr ctl)
{
  static uint16_t cnt = 0;

  Serial.printf(
      "%04X idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
      "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
      cnt++,
      ctl->index(),       // Controller Index
      ctl->dpad(),        // D-pad
      ctl->buttons(),     // bitmask of pressed buttons
      ctl->axisX(),       // (-511 - 512) left X Axis
      ctl->axisY(),       // (-511 - 512) left Y axis
      ctl->axisRX(),      // (-511 - 512) right X axis
      ctl->axisRY(),      // (-511 - 512) right Y axis
      ctl->brake(),       // (0 - 1023): brake button
      ctl->throttle(),    // (0 - 1023): throttle (AKA gas) button
      ctl->miscButtons(), // bitmask of pressed "misc" buttons
      ctl->gyroX(),       // Gyro X
      ctl->gyroY(),       // Gyro Y
      ctl->gyroZ(),       // Gyro Z
      ctl->accelX(),      // Accelerometer X
      ctl->accelY(),      // Accelerometer Y
      ctl->accelZ()       // Accelerometer Z
  );
}

void processGamepad(ControllerPtr ctl)
{

  is_key_update = true;
  if (ctl->a())
  {
    static int colorIdx = 0;
    switch (colorIdx % 3)
    {
    case 0:
      // Red
      ctl->setColorLED(255, 0, 0);
      break;
    case 1:
      // Green
      ctl->setColorLED(0, 255, 0);
      break;
    case 2:
      // Blue
      ctl->setColorLED(0, 0, 255);
      break;
    }
    colorIdx++;
  }

  if (ctl->b())
  {
    static int led = 0;
    led++;
    ctl->setPlayerLEDs(led & 0x0f);
  }

  if (ctl->x())
  {
    ctl->playDualRumble(0 /* delayedStartMs */, 250 /* durationMs */, 0x80 /* weakMagnitude */,
                        0x40 /* strongMagnitude */);
  }
  dumpGamepad(ctl);
  wheel.control(ctl->axisX(), ctl->axisY(), ctl->axisRX());
}

void processControllers()
{
  for (auto myController : myControllers)
  {
    if (myController && myController->isConnected() && myController->hasData())
    {
      if (myController->isGamepad())
      {
        processGamepad(myController);
      }
      else
      {
        Serial.printf("Unsupported controller class: %d\n", myController->getClass());
      }
    }
  }
}

bool buttonGetPressed(void)
{
  return !digitalRead(btn_pin);
}

void ledOff(void)
{
  digitalWrite(led_pin, HIGH);
}

void ledOn(void)
{
  digitalWrite(led_pin, LOW);
}

void ledToggle(void)
{
  digitalWrite(led_pin, !digitalRead(led_pin));
}

bool buttonIsForConnect(void)
{
  bool ret = false;
  static uint8_t state = 0;
  static uint32_t pre_time;

  switch (state)
  {
  case 0:
    if (buttonGetPressed())
    {
      pre_time = millis();
      state = 1;
    }
    break;

  case 1:
    if (!buttonGetPressed())
    {
      state = 0;
    }
    if (millis() - pre_time >= 3000)
    {
      Serial.println("Pressed For Conntect");
      state = 0;
      ret = true;
    }
    break;
  }

  return ret;
}

void updateConnect(void)
{
  enum
  {
    STATE_INIT,
    STATE_IDLE,
    STATE_WAIT_FOR_CONNECT,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
  };
  static uint8_t state = STATE_INIT;
  static uint32_t pre_time;

  switch (state)
  {
  case STATE_INIT:
    ledOff();
    pre_time = millis();
    state = STATE_IDLE;
    break;

  case STATE_IDLE:
    if (is_connected)
    {
      state = STATE_CONNECTED;
    }
    else
    {
      state = STATE_DISCONNECTED;
    }
    break;

  case STATE_WAIT_FOR_CONNECT:
    if (millis() - pre_time >= 100)
    {
      pre_time = millis();
      ledToggle();
    }
    if (is_connected)
    {
      state = STATE_CONNECTED;
    }
    break;

  case STATE_CONNECTED:
    if (is_key_update)
    {
      if (millis() - pre_time >= 200)
      {
        is_key_update = false;
      }
      if (millis() - pre_time <= 100)
      {
        ledOn();
      }
      else
      {
        ledOff();
      }
    }
    else
    {
      ledOn();
      pre_time = millis();
    }
    if (!is_connected)
    {
      state = STATE_DISCONNECTED;
    }
    break;

  case STATE_DISCONNECTED:
    if (millis() - pre_time >= 500)
    {
      pre_time = millis();
      ledToggle();
    }
    if (is_connected)
    {
      state = STATE_CONNECTED;
    }
    break;
  }

  if (buttonIsForConnect())
  {
    BP32.forgetBluetoothKeys();
    BP32.enableNewBluetoothConnections(true);
    is_connected = false;
    state = STATE_WAIT_FOR_CONNECT;
  }
}
void setup()
{
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t *addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false);
  pinMode(led_pin, OUTPUT);
  pinMode(btn_pin, INPUT_PULLUP);

  wheel.control(0, 0, 0);
}

void loop()
{
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();

  updateConnect();
  delay(10);
}
