#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>
#include <math.h>

RF24 radio(7, 8);
const uint8_t ADDRESS[][6] = {"00001", "00002"};
const unsigned int TIMEOUT = 200; // ms
const unsigned int TERM = 100; // ms
const unsigned int COUNT = 100;

typedef struct _Data {
  unsigned long timestamp;
} Data;

Data data;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);

  radio.openReadingPipe(1, ADDRESS[0]);
  radio.openWritingPipe(ADDRESS[1]);

  radio.stopListening();

  memset(&data, 0, sizeof(Data));
}

unsigned int ackedSockets = 0;
unsigned int totalLatency = 0;

void loop() {
  String command = Serial.readString();
  if (strcmp(command.c_str(), "start\n") == 0) {
    ackedSockets = 0;
    totalLatency = 0;

    for (int i = 0; i < COUNT; i++) {
      data.timestamp = millis();
      radio.write(&data, sizeof(Data));

      radio.startListening();

      unsigned long start = millis();
      //bool timeout = true;
      while (millis() < start + TIMEOUT) {
        if (radio.available()) {
          ackedSockets++;
          radio.read(&data, sizeof(Data));
          unsigned long end = millis();
          totalLatency += end - data.timestamp;
          break;
        }
      }

      //if (timeout) Serial.println("timeout");

      radio.stopListening();

      char str[100] = "";
      sprintf(str, "Progress: %d/%d", i+1, COUNT);
      Serial.println(str);

      delay(TERM);
    }

    Serial.println("\n=============");
    char str[100] = "";
    String acc = String((float)ackedSockets / COUNT * 100);
    String lat = String((float)totalLatency / ackedSockets);
    sprintf(str, "Acc: %s%%, Avg Lat: %sms, Timeout: %d/%d", acc.c_str(), lat.c_str(), COUNT - ackedSockets, COUNT);
    Serial.println(str);
    Serial.println("=============\n");
  }
}
