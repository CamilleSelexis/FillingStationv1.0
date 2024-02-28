void homePage(EthernetClient* client_pntr){
  client_pntr->print(Header);
  char buffer_loc[500];
  int homePageLen = strlen_P(homePageStr);
  int k = 0;
  for(int i=0;i<homePageLen;i++){
    
    buffer_loc[k] = pgm_read_byte_near(homePageStr+i);
    k++;
    if(k >= 500){
      client_pntr->write(buffer_loc,k);
      k= 0;
    }
  }
  client_pntr->write(buffer_loc,k);
  
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

void updateHomePage(EthernetClient* client_pntr){

  String updateValues = "";
  updateValues += String(millis()/1000);
  updateValues += "\r|";
  
  updateValues += String(valveState);
  updateValues += "\r|";
  updateValues += String(valvePort);
  updateValues += "\r|";

  updateValues += String(pumpState);
  updateValues += "\r|";
  updateValues += String(pumpFlow);
  updateValues += "\r|";


  updateValues += String(scaleState1);
  updateValues += "\r|";
  updateValues += String(scaleWeight1);
  updateValues += "\r|";
  updateValues += String(calibration_factor1);
  updateValues += "\r|";

  updateValues += String(scaleState2);
  updateValues += "\r|";
  updateValues += String(scaleWeight2);
  updateValues += "\r|";
  updateValues += String(calibration_factor2);
  updateValues += "\r|";

  updateValues += String(fillStationState);
  updateValues += "\r|";

  updateValues += String(rinseStationState);
  updateValues += "\r|";
  client_pntr->print( Header );
  client_pntr->print(updateValues);
  //client_pntr->print( tagPresent[0] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[1] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[2] ); 
  Serial.print(F("Update values sent : "));Serial.println(updateValues);
  while (client_pntr->read() != -1);
  client_pntr->stop();
  
}

void answerHTTP(EthernetClient* client_pntr) {
  client_pntr->print( Header );
  while (client_pntr->read() != -1);
  client_pntr->stop();
}
void getStatus(EthernetClient* client_pntr) {
  client_pntr->print( Header );
  client_pntr->print("Filling Station current status="+String(fillStationState));
  while (client_pntr->read() != -1);
  client_pntr->stop();
}

/*
 * Call this function to quickly connect to a client sends him a getStatus update
 */
void updateEthernetClient(){
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
      }
      //Serial.println("client connected");
      if(millis()-time_connection> TIMEOUT_ETH)
        client.stop();
    }
    Serial.println("Took "+String(millis()-time_start)+" ms to poll eth");
  }
}
