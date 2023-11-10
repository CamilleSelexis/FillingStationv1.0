
const char test[] PROGMEM = R"====(Hello World)====";
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
      //delay(100);
      //Serial.write(buffer_loc,k);
    }
  }
  client_pntr->write(buffer_loc,k);
  //Serial.write(buffer_loc,k);
  //strlcpy_P(buffer_loc,(char *)pgm_read_ptr(&(headerOK)),50);
  //client_pntr->print(buffer_str);
  //client_pntr->print(buffer_str);
  
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
  updateValues += String(scaleCalibration1);
  updateValues += "\r|";

  updateValues += String(scaleState1);
  updateValues += "\r|";
  updateValues += String(scaleWeight2);
  updateValues += "\r|";
  updateValues += String(scaleCalibration2);
  updateValues += "\r|";

  client_pntr->print( Header );
  client_pntr->print(updateValues);
  //client_pntr->print( tagPresent[0] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[1] );   client_pntr->print( "|" );  client_pntr->print( tagPresent[2] ); 
  Serial.println(F("Update values sent"));
  while (client_pntr->read() != -1);
  client_pntr->stop();
  
}
