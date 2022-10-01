#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


#define RL 47  //The value of resistor RL is 47K
#define m -0.263 //Enter calculated Slope 
#define b 0.42 //Enter calculated interceptm
#define Ro 20 //Enter found Ro value
#define MQ_sensor A0 //Sensor is connected to A4

SoftwareSerial BTSerial(7,8);
SoftwareSerial mySoftwareSerial(4, 5);
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  BTSerial.begin(9600);
  mySoftwareSerial.listen();
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
}

void loop() {
  boolean play_state = digitalRead(3);
  unsigned int Value = 0.0;
  float VRL; //Voltage drop across the MQ sensor
  float Rs; //Sensor resistance at gas concentration 
  float ratio; //Define variable for ratio
   
  VRL = analogRead(MQ_sensor)*(5.0/1023.0); //Measure the voltage drop and convert to 0-5V
  Rs = ((5.0*RL)/VRL)-RL; //Use formula to get Rs value
  ratio = Rs/Ro;  // find ratio Rs/Ro
 
  float ppm = pow(10, ((log10(ratio)-b)/m)); //use formula to calculate ppm
  Value = ppm;
  Serial.print(ppm);
  Serial.print(" ppm,");
  Serial.print(VRL);
  Serial.println(" v");

  
  
  if(ppm >=0.25){
    BTSerial.listen();
    BTSerial.print("1");
    Serial.println("예이예");
    myDFPlayer.volume(30);  
    if(play_state==HIGH){
      myDFPlayer.play(1);
     }   
    } 
  

  delay(1000);
}
