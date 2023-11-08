/*
This code controls the ERV and the pump
*/
#include "ElectricRotaryValve.h"
#include "KamoerPump.h"
#include "Arduino.h"

// Note: You also have to connect GND, 5V/VIO and VM.
//       A connection diagram can be found in the schematics.
#define MICROSTEPS  8 //8 microsteps by full steps
#define MICROSTEPS_PUMP 4
#define EN_PIN_ERV  7
#define EN_PIN_PUMP 6

#define HOME_PIN  9
#define PORT_PIN  8

#define STEP_PIN 3
#define DIR_PIN 2

double flow = 2; //ml/s
double deadVolume = 0;
ERV ElectricRotaryValve(HOME_PIN,PORT_PIN,EN_PIN_ERV,DIR_PIN,STEP_PIN,MICROSTEPS);
KamoerPump KamoerPump(EN_PIN_PUMP, DIR_PIN, STEP_PIN, MICROSTEPS_PUMP, flow, deadVolume);
void setup()
{
  //set pins
  pinMode(EN_PIN_ERV, OUTPUT);
  digitalWrite(EN_PIN_ERV, LOW); //deactivate driver
  pinMode(EN_PIN_PUMP, OUTPUT);
  digitalWrite(EN_PIN_PUMP, LOW); //deactivate driver

  pinMode(HOME_PIN,INPUT);
  pinMode(PORT_PIN,INPUT);
  pinMode(STEP_PIN,OUTPUT);
  digitalWrite(STEP_PIN,LOW);
  pinMode(DIR_PIN,OUTPUT);
  digitalWrite(DIR_PIN,HIGH);

  
  //init serial port
  Serial.begin(115200); //init serial port and set baudrate
  while(!Serial); //wait for serial port to connect (needed for Leonardo only)
  Serial.println("\nStart...");
  //init softSPI
  Serial.println("Init ERV");
  if(!ElectricRotaryValve.init_ERV())
    Serial.println("ERV initialization failed");
  Serial.println("Home pin = " +String(digitalRead(HOME_PIN)));
  
  Serial.println("Init pump");
  if(!KamoerPump.init_Pump()){
    Serial.println("Pump init failed");
  }
  Serial.println("Setup Done");
  
}

void loop()
{
 uint8_t value;
  if(Serial.available()){
    value = Serial.parseInt();
    if(value > 0 && value <9){
    Serial.print("You typed: " );
    Serial.println(value);
    if(!ElectricRotaryValve.openPort(value))
      Serial.println("Go to port failed");
    }
    if(value == 10){
      Serial.println("Will init ERV");
      if(!ElectricRotaryValve.init_ERV()){
        Serial.println("Init failed");
      }
      else{
        Serial.println("Init Success");
      }
    }
    if(value > 100){
      double volume = value-100;
      Serial.println("Will dispense " + String( volume) + " mL");
      KamoerPump.DispenseVolume(volume);
    }
  }

}
