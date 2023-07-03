// rf69_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_RF69 class. RH_RF69 class does not provide for addressing or
// reliability, so you should only use RH_RF69  if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example rf69_server.
// Demonstrates the use of AES encryption, setting the frequency and modem
// configuration
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

#include <SPI.h>
#include <RH_RF69.h>

// Singleton instance of the radio driver
//RH_RF69 rf69;
//RH_RF69 rf69(15, 16); // For RF69 on PJRC breakout board with Teensy 3.1
//RH_RF69 rf69(4, 2); // For MoteinoMEGA https://lowpowerlab.com/shop/moteinomega
RH_RF69 rf69(8, 7); // Adafruit Feather 32u4

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;
  if (!rf69.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)
  // No encryption
  if (!rf69.setFrequency(433.0))
    Serial.println("setFrequency failed");

  // If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
  // ishighpowermodule flag set like this:
  //rf69.setTxPower(14, true);

  // The encryption key has to be the same as the one in the server
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };
  rf69.setEncryptionKey(key);
}

uint8_t my_id = 10;
uint8_t my_frame_counter = 0;
uint8_t frame_counter[16];
uint8_t repeat_counter[16];

uint8_t data[128] = {NULL, NULL, NULL, 'K', 'W', 'O', 'N', ':',};
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);

void loop()
{
  if (Serial.available()) {
    int len_input = 0;
    data[0] = my_id;
    my_frame_counter++;
    data[1] = my_frame_counter;
    data[2] = 0; 
    while (Serial.available()) {
      data[7 + len_input] = Serial.read();
      len_input++;
    }
    data[7 + len_input] = NULL;
    len_input++;
    rf69.send(data, len_input + 7);
  }
  //Serial.println("Sending to rf69_server");
  // Send a message to rf69_server

  rf69.send(data, sizeof(data));

  rf69.waitPacketSent();
  // Now wait for a reply

  if (rf69.waitAvailableTimeout(500))
  {
    // Should be a reply message for us now
    if (rf69.recv(buf, &len))
    {
      buf[len] = NULL;
      Serial.print("got reply: ");
      Serial.println((char*)buf);
      int tmp_id = buf[0] - 1;
      Serial.print(tmp_id + 1);
      Serial.print(" ");
      if(frame_counter[tmp_id != buf[1]]){
        //처음들어옴
        Serial.println((char*)&buf[3]);
      }else{
        Serial.println("already recv");
      }
      uint8_t tmp_repeatcounter = buf[2];
      if(tmp_repeatcounter < 10){
        tmp_repeatcounter++;
        buf[2] = tmp_repeatcounter;
        rf69.send(buf, len + 1);
      }
    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
  }
  delay(400);
}
