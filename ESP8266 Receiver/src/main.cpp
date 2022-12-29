#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>

const unsigned short NUM_SERVO_CHANNELS = 4;
const unsigned short SERVO_PINS[NUM_SERVO_CHANNELS] = {D2, D3, D4, D5};
Servo servoChannels[NUM_SERVO_CHANNELS];

typedef struct message { unsigned short channelData[NUM_SERVO_CHANNELS]; } message;
message aMessage;
unsigned long previousMillisMessage = 0;
unsigned short MESSAGE_HEARTBEAT = 150;

void onDataReceiver(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&aMessage, incomingData, sizeof(aMessage));
  for (unsigned short i = 0; i < NUM_SERVO_CHANNELS; i++) {
    servoChannels[i].write(aMessage.channelData[i]);
  }
}
void setup() {
  Serial.begin(115200);
  WiFi.disconnect();
  ESP.eraseConfig();
  WiFi.mode(WIFI_STA);

  for (unsigned short i = 0; i < NUM_SERVO_CHANNELS; i++) {
    servoChannels[i].attach(SERVO_PINS[i]);
  }

  if (esp_now_init() != 0) {
    return;
  }
  esp_now_register_recv_cb(onDataReceiver);
}


void loop() {
  if (millis() - previousMillisMessage >= MESSAGE_HEARTBEAT) {
    for (unsigned short i = 0; i < NUM_SERVO_CHANNELS; i++) {
        servoChannels[i].write(90);
    }
    previousMillisMessage = millis();
  }
}