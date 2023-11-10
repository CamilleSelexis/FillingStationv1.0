/*
This code controls the ERV and the pump
*/
#include "ElectricRotaryValve.h"
#include "KamoerPump.h"
#include "Arduino.h"
#include "HX711.h"
#include "Ethernet5500.h"
#include <avr/pgmspace.h>
// Note: You also have to connect GND, 5V/VIO and VM.
//       A connection diagram can be found in the schematics.
#define MICROSTEPS  8 //8 microsteps by full steps
#define MICROSTEPS_PUMP 4
#define EN_PIN_ERV  7
#define EN_PIN_PUMP 6

#define HOME_PIN  9
#define PORT_PIN  8

#define STEP_PIN 5
#define DIR_PIN 4

double flow = 1; //ml/s
double deadVolume = 0;
ERV ElectricRotaryValve(HOME_PIN,PORT_PIN,EN_PIN_ERV,DIR_PIN,STEP_PIN,MICROSTEPS);
KamoerPump KamoerPump(EN_PIN_PUMP, DIR_PIN, STEP_PIN, MICROSTEPS_PUMP, flow, deadVolume);

#define DOUT  3
#define CLK  2

HX711 scale;

float calibration_factor = -420; //-7050 worked for my 440lb max scale setup
//take a long time to get a resting value -> due to swinging of the basket ?
long zero = -143564;

//Ethernet Variables
//Ethernet related settings
byte mac[] = {0x2C, 0xF7, 0xF1, 0x08, 0x39, 0x46};  //W5500 Mac address

int8_t ip_addr[4] = {192,168,1,110};
String strIP = "192.168.1.110";
IPAddress ip(192,168,1,110);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255, 255, 255, 0);
IPAddress myDns(8, 8, 8, 8); // google puble dns
EthernetServer server = EthernetServer(80);
#define TIMEOUT_ETH 1000

bool stringComplete = false;
String dataString = "";
String inputString = "";

long timeAlive;
int valveState = 1;
int valvePort = 0;

int pumpState = 1;
long pumpFlow = 100;

int scaleState1 = 0;
double scaleWeight1 = 0;
long scaleCalibration1 = 0;

int scaleState2 = 0;
double scaleWeight2 = 0;
long scaleCalibration2 = 0;
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
    Serial.println(F("ERV initialization failed"));
  Serial.println("Home pin = " +String(digitalRead(HOME_PIN)));
  
  Serial.println("Init pump");
  if(!KamoerPump.init_Pump()){
    Serial.println("Pump init failed");
  }
  Serial.println("Setup Done");
  //Init the Load Cell
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.set_offset(zero);
  //init Ehternet server
  inputString.reserve(200);

  if(!initEthernet()) Serial.println("Error on Ethernet");
}
void loop()
{
  uint8_t value;
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  /*
  Serial.print("Reading: ");
  Serial.print(scale.get_units(5), 1);
  Serial.print(" g"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  Serial.print(" ADC value: ");Serial.print(scale.get_value(10));
  Serial.print(" calibration_factor: ");
  Serial.print(calibration_factor);
  Serial.println();
  */
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

  //Ethernet Client check
  EthernetClient client = server.available();
  EthernetClient *client_pntr = &client;
  if(client){
    String currentLine = "";
    long time_connection = millis();
    while(client.connected()){
      if(client.available()) {
        currentLine = ""; //reset currentLine
        //Read the first Line
        char c = client.read();
        while(!(c== '\n' || c == ' ' || c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
        }
        if(currentLine == "home"){homePage(client_pntr);}
        if(currentLine == "updateStartPage"){Serial.println(F("updateHomePage"));updateHomePage(client_pntr);}
        
        if(currentLine == "resetController"){software_reset();}
      }
      //Serial.println("client connected");
      if(millis()-time_connection> TIMEOUT_ETH)
        client.stop();
      delay(20);
    }
  }

}

//Reset Arduino
void software_reset() {
  asm volatile (" jmp 0");  
}

//Init Ethernet Server
bool initEthernet(){
  Serial.println(F("Ethernet Coms starting..."));
  Ethernet.begin(mac, ip, myDns, gateway, subnet);  //Start the Ethernet coms
  // Check for Ethernet hardware present
  // Start the server
  server.begin();           //"server" is the name of the object for comunication through ethernet
  Serial.print(F("Ethernet server connected. Server is at "));
  //Serial.println(Ethernet.localIP());         //Gives the local IP through serial com
  Serial.println(strIP);
  return true;
}
