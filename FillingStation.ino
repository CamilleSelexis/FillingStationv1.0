void initializeFillingStation(){
  PRINTLN("Initialize the filling station");
  //Init valve
  initializeValve();

  //Init Pump
  initializePump();

  //Init the Load Cell
  initializeScale1();
  initializeScale2();

  //init rinse Station
  initializeRinseStation();

  PRINTLN("Initialization done");
}

void primeLinesFillingStation(){
  if(fillStationState == 1){
    flushFillingStation();
    goToPort(MEDIAPORT1);
    updateEthernetClient();
    dispenseVolume(primeVolMedia1);
    updateEthernetClient();
    goToPort(MEDIAPORT2);
    updateEthernetClient();
    dispenseVolume(primeVolMedia2);
    updateEthernetClient();
    goToPort(H2OPORT);
    updateEthernetClient();
    dispenseVolume(primeVolH2O);
    updateEthernetClient();
    goToPort(ETHPORT);
    updateEthernetClient();
    dispenseVolume(primeVolEth);
    updateEthernetClient();
    goToPort(H2OPORT);
    updateEthernetClient();
    dispenseVolume(deadVolume*2);
    updateEthernetClient();
    goToPort(AIRPORT);
    updateEthernetClient();
    dispenseVolume(deadVolume*2);
    updateEthernetClient();
  }
  else{
    PRINTLN("Filling Station is not ready");
  }
}
void dispenseMediaVol(float dispenseVol){
  //Update all states
  getFillingStationState();
  double weightLeft = 0;
  int mediaPort = 0;
  if(fillStationState == 1){
    //if no scale active, select the one with the least volume
    if(!(scaleActive1 || scaleActive2)){
      if(!selectScaleAuto()){
        PRINTLN("ERROR selecting scale, no scales available");
        return;
      }
    }
    if(scaleActive1){
      weightLeft = scaleWeight1;
      mediaPort = MEDIAPORT1;
    }
    else{
      weightLeft = scaleWeight2;
      mediaPort = MEDIAPORT2;
    }
    //check that there is enough media volume
    if((dispenseVol+deadVolume) < (weightLeft-deadVolumePouch)){
      goToPort(mediaPort);
      updateEthernetClient();
      dispenseVolume(dispenseVol);
      updateEthernetClient();
      goToPort(4);
      dispenseVolume(deadVolume);
      flushFillingStation();
      updateEthernetClient();
      PRINTLN("Dispense Media completed");
    }
    else{
      PRINTLN("Not enough volume available in the pouch, change pouch");
    }
  }
  else{
    PRINTLN("Filling Station is not ready");
  }
}

void cleanFillingStation(){
  getFillingStationState();
  if(fillStationState == 1){
    PRINTLN("Filling Station cleaning routine start");
    goToPort(AIRPORT);
    dispenseVolume(deadVolume*2);
    goToPort(ETHPORT);
    dispenseVolume(deadVolume*2);
    goToPort(H2OPORT);
    dispenseVolume(deadVolume*2);
    goToPort(AIRPORT);
    dispenseVolume(deadVolume*2);
  }
  else{
    PRINTLN("Filling Station is not ready");
  }
}

void flushFillingStation(){
  getFillingStationState();
  if(fillStationState == 1){
    PRINTLN("Filling Station air flush start");
    goToPort(AIRPORT);
    dispenseVolume(deadVolume*2);
  }
  else{
    PRINTLN("Filling Station is not ready");
  }
}

void getFillingStationState(){
  getValveState();
  getPumpState();
  getRinseStationState();

  updateScalesValues();
  getScaleState1();
  getScaleState2();

  if(valveInit && pumpInit && (scaleInit1 || scaleInit2)){
    if(valveBusy || pumpBusy){
      fillStationState = 2; //fillStation is busy
    }
    else{
      fillStationState = 1; //fillStation is ready
    }
  }
  else{
    fillStationState = 0; //fillStation is not init
  }
}
