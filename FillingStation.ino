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
    volumeMultiplier = 1.0;
    //Set pump flow rate to 500uL/s to avoid leakage on the tips
    setFlowRate(500);
    fillStationBusy = true;
    flushFillingStation();
    double weightStart1 = scaleWeight1;
    goToPort(MEDIAPORT1);
    updateEthernetClient();
    dispenseVolume(primeVolMedia1);
    getFillingStationState();
    double weightEnd1 = weightStart1-scaleWeight1;
    updateEthernetClient();
    goToPort(MEDIAPORT2);
    updateEthernetClient();
    dispenseVolume(primeVolMedia2);
    updateEthernetClient();
    /*goToPort(H2OPORT);
    updateEthernetClient();
    dispenseVolume(primeVolH2O);*/
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
    if(scaleActive1)
      goToPort(MEDIAPORT1);
    else if(scaleActive2){
      goToPort(MEDIAPORT2);
    }
    getFillingStationState();
    double weightStart2 = scaleWeight1;
    dispenseVolume(deadVolume);
    getFillingStationState();
    double weightEnd2 = weightStart2-scaleWeight1;
    PRINTLN("Volume variation 1 = "+String(weightEnd1)+" // Volume variation 2 = "+String(weightEnd2));
    //volumeMultiplier = 1/((weightEnd1/deadVolume+weightEnd2/deadVolume)/2);
    PRINTLN("volume multiplier should be set to "+String(volumeMultiplier));
    setFlowRate(pumpFlow);
    fillStationBusy = false;
  }
  else{
    PRINTLN("Filling Station is not ready");
  }
}
void dispenseMediaVol(float dispenseVol){
  //Update all states
  getFillingStationState();
  double weightLeft = 0;
  double weightStart = 0;
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
      //The volume dispensed depends on the pressure inside the media pouch so on its available volume
      weightLeft = scaleWeight1;
      mediaPort = MEDIAPORT1;
      //dispenseVol = dispenseVol-0.4*pow(((scaleWeight1-2800)/2200),3);
      weightStart = scaleWeight2;
    }
    else{
      weightLeft = scaleWeight2;
      mediaPort = MEDIAPORT2;
      //dispenseVol = dispenseVol-0.4*pow(((scaleWeight2-2800)/2200),3);
      weightStart = scaleWeight1;
    }
    //check that there is enough media volume
    if((dispenseVol+deadVolume) < (weightLeft-deadVolumePouch)){
      fillStationBusy = true;
      goToPort(mediaPort);
      updateEthernetClient();
      if(dispenseVol > 2){
        dispenseVolume(dispenseVol*0.75);
        updateEthernetClient();
        getFillingStationState();
        lastDispense = weightLeft - (scaleActive1?scaleWeight1:scaleWeight2);
        weightVar = weightStart - (scaleActive1?scaleWeight2:scaleWeight1);
        lastDispenseCorrected = lastDispense-weightVar;
        double dispense2  = dispenseVol*0.25 + (dispenseVol*0.75-lastDispenseCorrected);
        dispenseVolume(dispense2);
      }
      else{
        dispenseVolume(dispenseVol);
      }
     
      fillStationBusy = false;
      PRINTLN("Dispense Media completed");
      getFillingStationState();
      lastDispense = weightLeft - (scaleActive1?scaleWeight1:scaleWeight2);
      weightVar = weightStart - (scaleActive1?scaleWeight2:scaleWeight1);
      lastDispenseCorrected = lastDispense-weightVar;
      dispenseVolume(-0.15); //aspirate the droplet
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
    fillStationBusy = true;
    PRINTLN("Filling Station cleaning routine start");
    goToPort(ETHPORT);
    dispenseVolume(deadVolume*2);
    goToPort(AIRPORT);
    dispenseVolume(deadVolume*2);
    goToPort(H2OPORT);
    dispenseVolume(deadVolume*2);
    goToPort(AIRPORT);
    dispenseVolume(deadVolume*2);
    fillStationBusy = false;
  }
  else{
    PRINTLN("Filling Station is not ready");
  }
}

void flushFillingStation(){
  getFillingStationState();
  if(fillStationState == 1){
    fillStationBusy = true;
    PRINTLN("Filling Station air flush start");
    goToPort(AIRPORT);
    dispenseVolume(deadVolume*2);
    fillStationBusy = false;
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
    if(valveBusy || pumpBusy || fillStationBusy){
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
