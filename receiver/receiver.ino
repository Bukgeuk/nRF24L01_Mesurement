#include <SPI.h>
//#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);
const uint8_t ADDRESS[][6] = {"00001", "00002"};

typedef struct _Data {
  unsigned long timestamp;
} Data;

Data data;

void setup() {
  Serial.begin(9600);
  
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);

  radio.openWritingPipe(ADDRESS[0]);
  radio.openReadingPipe(1, ADDRESS[1]);
  
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data));

    radio.stopListening();
    
    radio.write(&data, sizeof(Data));

    Serial.print("timestamp: ");
    Serial.println(data.timestamp);

    radio.startListening();
  }
}
