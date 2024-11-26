#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "can.h"

const char* ssid = "...";
const char* password = "...";

AsyncWebServer server(80);

const int ledPin = 2;
const int buttonPin = 0; 
bool ledState = false;   
bool lastButtonState = HIGH; 
unsigned long lastDebounceTime = 0; 
const unsigned long debounceDelay = 50; 

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32-C3-CAN LED Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html { font-family: Arial; display: inline-block; text-align: center; }
    h2 { font-size: 2rem; }
    p { font-size: 1.5rem; }
    .button { 
      padding: 16px 40px; 
      font-size: 24px; 
      margin: 2px; 
      cursor: pointer; 
      border: none; 
      border-radius: 5px;
    }
    .button-on { background-color: #4CAF50; color: white; }
    .button-off { background-color: #f44336; color: white; }
  </style>
  <script>
    function updateLED(state) {
      const button = document.getElementById('toggleButton');
      const ledStateText = document.getElementById('ledState');
      if (state === 'ON') {
        button.className = 'button button-on';
        button.innerHTML = 'Turn OFF';
        ledStateText.innerHTML = 'LED is ON';
      } else {
        button.className = 'button button-off';
        button.innerHTML = 'Turn ON';
        ledStateText.innerHTML = 'LED is OFF';
      }
    }

    setInterval(() => {
      fetch('/state')
        .then(response => response.text())
        .then(state => updateLED(state));
    }, 500);

    function toggleLED() {
      fetch('/toggle')
        .then(response => response.text())
        .then(state => updateLED(state));
    }
  </script>
</head>
<body>
  <h2>ESP32-C3-CAN LED Control</h2>
  <p id="ledState">LED is %LED_STATE%</p>
  <button id="toggleButton" onclick="toggleLED()" class="button button-%BUTTON_CLASS%">
    Turn %BUTTON_ACTION%
  </button>
</body>
</html>
)rawliteral";

String processor(const String& var) {
  if (var == "LED_STATE") {
    return ledState ? "ON" : "OFF";
  } else if (var == "BUTTON_CLASS") {
    return ledState ? "on" : "off";
  } else if (var == "BUTTON_ACTION") {
    return ledState ? "OFF" : "ON";
  }
  return String();
}

void toggleLED() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState?LOW:HIGH);

}
void setLED(bool on) {
  ledState = on;
  digitalWrite(ledPin, ledState?LOW:HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(buttonPin, INPUT_PULLUP); 
  
  canInit();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());



  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/state", HTTP_GET, [](AsyncWebServerRequest* request) {
    String state = ledState ? "ON" : "OFF";
    request->send(200, "text/plain", state);
  });

  server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest* request) {
    toggleLED();
    request->send(200, "text/plain", ledState ? "ON" : "OFF");
  });
  server.begin();
}

void loop() {  
  int buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (lastDebounceTime != 0 && ((millis() - lastDebounceTime) > debounceDelay)) {
    if (buttonState == LOW) {
      toggleLED(); 
    }
  }
  lastButtonState = buttonState;
  
  CanFrame rxFrame;
  if(canUpdate(&rxFrame)) {
    if(rxFrame.identifier == 0x501) {
      switch(rxFrame.data[0]) {
      case 0:
        toggleLED(); 
        break;
      case 1:
        setLED(1); 
        break;
      case 2:
      default:
        setLED(0); 
        break;
      }
    }
  }
  
  static uint32_t lastStamp = 0;  
  if(millis() - lastStamp > 1000) { 
      lastStamp = millis();
      canSendLedState(ledState);
  }
}
