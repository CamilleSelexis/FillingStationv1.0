void initializePump(){
  if(!KamoerPump.init_Pump()){
    PRINTLN("Pump init failed");
    pumpInit = false;
    pumpBusy = false;
  }
  else{
    PRINTLN("Pump initialized");
    pumpInit = true;
    pumpBusy = false;
  }
  getPumpState();
}

void setFlowRate(float flowRate){
  if(pumpInit && !pumpBusy)
  {
    if(flowRate > 0 && flowRate <= MAXFLOW){
      double flowML = flowRate/1000;
      PRINT("Set pump flow rate to : ");PRINT(flowML);PRINTLN(" mL/s");
      KamoerPump.setFlow(flowML);
      pumpFlow = flowRate;
    }
    else{
      PRINTLN("Flow rate outside of boundaries, 0-"+ String(MAXFLOW));
    }
  }
  else{
    PRINTLN("Init pump before setting the flow rate and wait for it to be idle");
  }
}

void dispenseVolume(float dispenseVol){
  dispenseVol = dispenseVol*volumeMultiplier;
  if(pumpInit && !pumpBusy){
    if(dispenseVol >0 && dispenseVol<=MAXDISPENSE){
      PRINTLN("The pump will dispense " + String(dispenseVol) + " mL");
      pumpBusy = true;
      KamoerPump.DispenseVolume(dispenseVol);
      pumpBusy = false;
    }
    else{
      PRINTLN("Dispense volume outside of boundaries, 0-" + String(MAXDISPENSE));
    }
  }
  else{
    PRINTLN("Init pump before sending dispense command and wait for it to be idle");
  }
}

void getPumpState(){
  if(pumpInit){
    if(pumpBusy){
      pumpState = 2;
    }
    else
      pumpState = 1;
  }
  else
    pumpState = 0;
}
