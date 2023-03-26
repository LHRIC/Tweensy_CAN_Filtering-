#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;
#include <SPI.h>
#include <SD.h>
const int chipSelect = BUILTIN_SDCARD;
uint16_t throttlePos;
void setup(void) {
  Serial.begin(115200); 
  delay(400);
  pinMode(PUN_D7, INPUT_PULLUP); 
  pinMode(6, OUTPUT); digitalWrite(6, LOW); /* optional tranceiver enable pin */
  Can0.begin();
  Can0.setBaudRate(1000000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  Can0.onReceive(canSniff);
  Can0.mailboxStatus();
  Serial.print("turning on SD card");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1); 
  }
  Serial.println("card worky");
}

void canSniff(const CAN_message_t &msg) {
  if (!digitalRead(PIN_D7)) {
  File dataFile = SD.open("0x640data.txt", FILE_WRITE);
  if(msg.id == 0x640) {
  /*
  Serial.print("MB "); Serial.print(msg.mb);
  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print("  LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Buffer: ");
  */
    throttlePos = (msg.buf[msg.len - 2] << 8) | msg.buf[msg.len - 1];
    Serial.println(throttlePos * 0.1); 
    //Serial.print(msg.buf[msg.len - 2]);
    //Serial.print(",");
    //Serial.println(msg.buf[msg.len - 1]);
    if (dataFile) {
      dataFile.print(millis());
      dataFile.print(": ");
      dataFile.print(msg.id,HEX); 
      dataFile.print(", "); 
      dataFile.println(throttlePos, DEC);
      dataFile.println(); 
      dataFile.close();
    }
  }
  } else {
    Serial.println("error! button no worky :(");
  }
  
}

void loop() {
  Can0.events();
  
  static uint32_t timeout = millis();
  if ( millis() - timeout > 200 ) {
    CAN_message_t msg;
    msg.id = 0x640;
    for ( uint8_t i = 0; i < 8; i++ ) msg.buf[i] = i + 1;
    //Can0.write(msg);
    timeout = millis();
  }

}
