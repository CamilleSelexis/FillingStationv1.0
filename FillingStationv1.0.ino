/*
This code controls the ERV and the pump
*/
#include "ElectricRotaryValve.h"
#include "KamoerPump.h"
#include "Arduino.h"
#include "HX711.h"
#include "Ethernet5500.h"
#include <avr/pgmspace.h>
//#include <TimerInterrupt.h>
// Note: You also have to connect GND, 5V/VIO and VM.
//       A connection diagram can be found in the schematics.

#define DEBUG 1
#if DEBUG
#define PRINT(str) Serial.print(str);
#define PRINTLN(str) Serial.println(str);
#else if
#define PRINT(str)
#endif

#define MICROSTEPS  8 //8 microsteps by full steps
#define MICROSTEPS_PUMP 8

#define HOME_PIN  30
#define PORT_PIN  31

#define DIR_PUMP   22
#define STEP_PUMP    23
#define EN_PUMP     24

#define DIR_ERV    26
#define STEP_ERV     27
#define EN_ERV      28


#define MAXFLOW 1200
#define MAXDISPENSE 100

//Valve used port
#define AIRPORT   1
#define H2OPORT   6
#define ETHPORT   7
#define MEDIAPORT1 2
#define MEDIAPORT2 3

float primeVolMedia1 = 10; //mL - This volume is the volume between the pouch and the pump
float primeVolMedia2 = 10; //mL
float primeVolH2O = 10; //mL
float primeVolEth = 10; //mL

double flow = 1; //ml/s
double deadVolume = 10; //mL - This volume is the one used to flush/clean the tubings, corresponds to the volume between the pump and the dispenser
double deadVolumePouch = 320;//g - Corresponds to the volume+weight of the empty bag that cannot be used (50mL) to be safe ?
ERV ElectricRotaryValve(HOME_PIN,PORT_PIN,EN_ERV,DIR_ERV,STEP_ERV,MICROSTEPS);
KamoerPump KamoerPump(EN_PUMP, DIR_PUMP, STEP_PUMP, MICROSTEPS_PUMP, flow, deadVolume);
double volumeMultiplier = 1.0; //This parameter can be changed directly on the UI allowing the User to easily adjust the amount of volume delivered
#define SCALE1_DOUT  38
#define SCALE1_CLK  39
#define SCALE2_DOUT  40
#define SCALE2_CLK    41
HX711 scale1;//Scale on the Left
HX711 scale2;//Scale on the right

long calibration_factor1 = -427;
long calibration_factor2 = -427; 

long zero1 = -134500;//163450
long zero2 = -109350;

bool scaleInit1 = false;
bool scaleActive1 = false;
int scaleState1 = 0;
double scaleWeight1 = 0;

bool scaleInit2 = false;
bool scaleActive2 = false;
int scaleState2 = 0;
double scaleWeight2 = 0;

double scaleDeadWeight = 50; //To be properly measured
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
bool valveInit = false;
bool valveBusy = false;
int valveState = 0;
int valvePort = 0;

bool pumpInit = false;
bool pumpBusy = false;
int pumpState = 0;
float pumpFlow = 1000;

int fillStationState = 0;
bool fillStationBusy = false;

int rinseStationState = 0;
bool rinseBusy = false;
bool rinse1Busy = false;
bool rinse2Busy = false;
bool rinseAirBusy = false;
bool rinseUVCBusy = false;
bool rinseInit = false;
long timeRinseStart1 = 0;
long timeRinseStart2 = 0;
long timeRinseAir = 0;
long timeUVCLed = 0;
float dispenseAirTime = 10000;
float dispenseClean1Time = 10000;
float dispenseClean2Time = 10000;
float UVCLedONTime = 10000;
float flowClean1 = 3.7; //mL/s
float flowClean2 = 3.7; //mL/s

bool UVCOn = false;
bool toggleLed = true;

#define RINSE_PUMP1 8
#define RINSE_PUMP2 7
#define RINSE_AIRVALVE 6
#define LED_UVC   9

#define TIMEPUMP 10000 //ms * 4mL/s = 50mL
#define TIMEAIR 5000 //ms
#define TIMEDRIPPING 25000 //ms
#define TIMEUVC 30000 //ms

#define DISPENSECLEANMIN 0 //ms min time of dispense
#define DISPENSECLEANMAX 15000 //ms max time for dispense

double lastDispense = 0;
double lastDispenseCorrected = 0;
double weightVar = 0;
void setup()
{
  //set pins
  pinMode(EN_ERV, OUTPUT);
  digitalWrite(EN_ERV, LOW); //deactivate driver
  pinMode(EN_PUMP, OUTPUT);
  digitalWrite(EN_PUMP, LOW); //deactivate driver

  pinMode(HOME_PIN,INPUT);
  pinMode(PORT_PIN,INPUT);
  pinMode(STEP_ERV,OUTPUT);
  digitalWrite(STEP_ERV,LOW);
  pinMode(DIR_ERV,OUTPUT);
  digitalWrite(DIR_ERV,HIGH);

  pinMode(STEP_PUMP,OUTPUT);
  digitalWrite(STEP_PUMP,LOW);
  pinMode(DIR_PUMP,OUTPUT);
  digitalWrite(DIR_PUMP,HIGH);
  
  //init serial port
  Serial.begin(115200); //init serial port and set baudrate
  //while(!Serial); //wait for serial port to connect (needed for Leonardo only)
  PRINTLN("\nStart...");
  initializeFillingStation();

  //init Ehternet server
  inputString.reserve(50);

  if(!initEthernet()) PRINTLN("Error on Ethernet");
  PRINTLN("Setup Done");
}
void loop()
{
  //Get current status of the filling station
  getFillingStationState();

  //Automatically disable the rinse station functions after their enable time is passed
  if(rinseBusy){
    long nowTime = millis();
    if(rinse1Busy && (nowTime-timeRinseStart1) > dispenseClean1Time){
      rinse1OFF();
    }
    if(rinse2Busy && (nowTime-timeRinseStart2) > dispenseClean2Time){
      rinse2OFF();
    }
    if(rinseAirBusy && (nowTime-timeRinseAir) > dispenseAirTime){
      rinseAirOFF();
    }
    if(rinseUVCBusy && (nowTime-timeUVCLed) > UVCLedONTime){
      disableUVCLed();
    }
  }
  //Ethernet client Check
  long time_start = millis();
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
        //while(!(c== '\n' || c == ' ' || c == '/' || c == -1)){
        while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
        }
        if(currentLine=="home"){homePage(client_pntr);}
        if(currentLine=="getStatus"){getStatus(client_pntr);}
        if(currentLine=="updateStartPage"){PRINTLN("updateHomePage");updateHomePage(client_pntr);}
        //High level commands
        if(currentLine=="initializeFillStation") {PRINTLN("initializeFillStation");answerHTTP(client_pntr);initializeFillingStation();}
        if(currentLine=="primeLinesFillStation") {PRINTLN("primeLinesFillStation");answerHTTP(client_pntr);primeLinesFillingStation();}
        if(currentLine=="dispenseMediaVol") {
          PRINTLN("dispenseMediaVol");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          float mediaVol = currentLine.toFloat();
          PRINTLN(mediaVol);
          answerHTTP(client_pntr);
          dispenseMediaVol(mediaVol);
        }
        if(currentLine=="cleanFillStation") {PRINTLN("cleanFillStation");answerHTTP(client_pntr);cleanFillingStation();}
        if(currentLine=="flushFillStation"){ PRINTLN("flushFillStation");answerHTTP(client_pntr);flushFillingStation();}
        if(currentLine=="rinseTip"){PRINTLN("RinseTip");answerHTTP(client_pntr);rinseTip();}
        //Low level manual commands
        if(currentLine=="initializeValve") {PRINTLN("initializeValve");answerHTTP(client_pntr);initializeValve();}
        if(currentLine=="goToPort") {
          PRINTLN("goToPort");
          currentLine = "";
          c= client.read();
          int portNumber = c -48;
          PRINTLN(portNumber);
          answerHTTP(client_pntr);
          goToPort(portNumber);
        }
        if(currentLine=="initializePump") {PRINTLN("initializePump");answerHTTP(client_pntr);initializePump();}
        if(currentLine=="setFlow") {
          PRINTLN("setFlow");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          float flowRate = currentLine.toFloat();
          PRINTLN(flowRate);
          answerHTTP(client_pntr);
          setFlowRate(flowRate);
        }
        if(currentLine=="dispenseVol") {
          PRINTLN("dispenseVol");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          float dispenseVol = currentLine.toFloat();
          PRINTLN(dispenseVol);
          answerHTTP(client_pntr);
          dispenseVolume(dispenseVol);
        }
        if(currentLine=="initializeScale1") {PRINTLN("initializeScale1");answerHTTP(client_pntr);initializeScale1();}
        if(currentLine=="selectScale1") {PRINTLN("selectScale1");answerHTTP(client_pntr);selectScale1();}
        if(currentLine=="tareScale1") {PRINTLN("tareScale1");answerHTTP(client_pntr);tareScale1();}
        if(currentLine=="setCalibration1") {
          PRINTLN("setCalibration1");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          calibration_factor1 = currentLine.toInt();
          PRINTLN(calibration_factor1);
          setCalibration1();
          answerHTTP(client_pntr);
          
        }
        if(currentLine=="initializeScale2") {PRINTLN(F("initializeScale2"));answerHTTP(client_pntr);initializeScale2();}
        if(currentLine=="selectScale2") {PRINTLN("selectScale2");answerHTTP(client_pntr);selectScale2();}
        if(currentLine=="tareScale2") {PRINTLN(F("tareScale2"));answerHTTP(client_pntr);tareScale2();}
        if(currentLine=="setCalibration2") {
          PRINTLN("setCalibration2");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          calibration_factor2 = currentLine.toInt();
          PRINTLN(calibration_factor2);
          setCalibration2();
          answerHTTP(client_pntr);
        }
        if(currentLine =="selectScale0"){
          PRINTLN("selectScale0");
          answerHTTP(client_pntr);
          if(!selectScaleAuto()){
            PRINTLN("ERROR selecting scale");
            }
        }
        if(currentLine == "getScaleWeight"){
          PRINTLN("getScaleWeight");
          answerHTTPWeight(client_pntr);
        }
        //Rinse STATION
        if(currentLine=="dispenseAir") {
          PRINTLN("dispenseAir");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          dispenseAirTime = currentLine.toFloat()*1000; //from s to ms
          PRINTLN(dispenseAirTime);
          dispenseAir();
          answerHTTP(client_pntr);
        }
        if(currentLine=="dispenseClean1") {
          PRINTLN("dispenseClean1");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          float dispenseVol = currentLine.toFloat();
          PRINTLN(dispenseVol);
          dispenseClean1(dispenseVol);
          answerHTTP(client_pntr);
        }
        if(currentLine=="dispenseClean2") {
          PRINTLN("dispenseClean2");
          currentLine = "";
          c= client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          float dispenseVol = currentLine.toFloat();
          PRINTLN(dispenseVol);
          dispenseClean2(dispenseVol);
          answerHTTP(client_pntr);
        }
        if(currentLine=="enableUVC") {
          PRINTLN("enableUVC");
          currentLine = "";
          c=client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          UVCLedONTime = currentLine.toFloat()*1000; //convert from s to ms
          PRINTLN(UVCLedONTime);
          enableUVCLed();
          answerHTTP(client_pntr);
        }
        if(currentLine =="setVolumeMultiplier") {
          PRINTLN("setVolumeMultiplier");
          currentLine = "";
          c=client.read();
          while(!(c== '\n' || c == ' '|| c == '/' || c == -1)){
          currentLine += c;
          c = client.read();
          }
          volumeMultiplier = currentLine.toFloat();
          PRINTLN(volumeMultiplier);
          answerHTTP(client_pntr);
        }
        if(currentLine=="resetController"){answerHTTP(client_pntr); software_reset();}
      }
      //Serial.println("client connected");
      if(millis()-time_connection> TIMEOUT_ETH)
        client.stop();
      delay(20);
    }
    Serial.println("Took "+String(millis()-time_start)+" ms to poll eth");
  }
}

//Reset Arduino
void software_reset() {
  asm volatile (" jmp 0");  
}
//Init Ethernet Server
bool initEthernet(){
  PRINTLN(F("Ethernet Coms starting..."));
  Ethernet.begin(mac, ip, myDns, gateway, subnet);  //Start the Ethernet coms
  // Check for Ethernet hardware present
  // Start the server
  server.begin();           //"server" is the name of the object for comunication through ethernet
  PRINT(F("Ethernet server connected. Server is at "));
  //Serial.println(Ethernet.localIP());         //Gives the local IP through serial com
  PRINTLN(strIP);
  return true;
}
