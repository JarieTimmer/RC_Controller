#include<Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

const unsigned short NUM_ADC_PINS = 4;
const unsigned short ADC_PINS[NUM_ADC_PINS] = {GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26};
uint8_t receiverMacAddress[] = {0x94, 0xB9, 0x7E, 0x10, 0x55, 0x3C};


typedef struct message {
  unsigned short channelData[NUM_ADC_PINS];
} message;
message aMessage;
esp_now_peer_info_t receiverInfo;


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}
 
void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(receiverInfo.peer_addr, receiverMacAddress, 6);
  receiverInfo.channel = 0;  
  receiverInfo.encrypt = false;
      
  if (esp_now_add_peer(&receiverInfo) != ESP_OK){
    return;
  }
}

unsigned long previousMillis = 0;
const unsigned short MESSAGE_INTERVAL = 20;
 
void loop() {
  if  (millis() - previousMillis >= MESSAGE_INTERVAL) {
    for (unsigned short i = 0; i < NUM_ADC_PINS; i++) {
      aMessage.channelData[i] = map(analogRead(ADC_PINS[i]), 0, 4095, 30, 150);
  }
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &aMessage, sizeof(aMessage)); 
  previousMillis = millis();
  }
}