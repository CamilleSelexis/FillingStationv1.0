void initializeValve(){
  if(!ElectricRotaryValve.init_ERV()){
    PRINTLN(F("ERV initialization failed"));
    valveInit = false;
    valveBusy = false;
  }
  else{
    valveInit = true;
    valveBusy = false;
    valvePort = 1;
  }
  getValveState();
}

void goToPort(int portNumber){

  if(valveInit && !valveBusy){
    if(portNumber > 0 && portNumber <9){
      PRINTLN("Will go to port "+String(portNumber));
      if(!ElectricRotaryValve.openPort(portNumber)){
        PRINTLN("Go to port failed");
      }
      else{
        valvePort = portNumber;
        PRINTLN("Go to port success");
      }
    }
    else{
      PRINTLN("Port outside of boundaries 1-8");
    }
  }
  else{
    PRINTLN("Init valve and wait for it to be done before sending a command");
  }
}

void getValveState(){
  if(valveInit){
    if(valveBusy){
      valveState = 2;
    }
    else
      valveState = 1;
  }
  else
    valveState = 0;
}
