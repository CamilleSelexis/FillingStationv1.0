void getRinseStationState(){
  if(rinse1Busy || rinse2Busy || rinseAirBusy ||rinseUVCBusy)
    rinseBusy = true;
  else
    rinseBusy = false;
  if(rinseInit){
    if(rinseBusy){
      rinseStationState = 2;
    }
    else{
      rinseStationState = 1;
    }
  }
  else
    rinseStationState = 0;
}

void initializeRinseStation(){
  pinMode(RINSE_PUMP1,OUTPUT);
  pinMode(RINSE_PUMP2,OUTPUT);
  pinMode(RINSE_AIRVALVE,OUTPUT);
  pinMode(LED_UVC,OUTPUT);
  digitalWrite(RINSE_PUMP1,LOW);
  digitalWrite(RINSE_PUMP2,LOW);
  digitalWrite(RINSE_AIRVALVE,LOW);
  digitalWrite(LED_UVC,LOW);
  rinseBusy = false;
  rinse1Busy = false;
  rinse2Busy = false;
  rinseAirBusy = false;
  rinseInit = true;
}

void rinseTip(){
  long time_rinse_start = millis();
  long time_UVC_start = millis();
  rinse1Busy = true;
  rinse2Busy = true;
  digitalWrite(RINSE_PUMP1,HIGH);
  digitalWrite(RINSE_PUMP2,HIGH);
  //digitalWrite(LED_UVC,HIGH);
  while(millis()-time_rinse_start<TIMEPUMP){
    
  }
  digitalWrite(RINSE_PUMP1,LOW);
  digitalWrite(RINSE_PUMP2,LOW);
  rinse1Busy = false;
  rinse2Busy = false;
  delay(100);
  time_rinse_start = millis();
  rinseAirBusy = true;
  digitalWrite(RINSE_AIRVALVE,HIGH);
  while(millis()-time_rinse_start<TIMEAIR){
    
  }
  digitalWrite(RINSE_AIRVALVE,LOW);
  while(millis()-time_UVC_start < TIMEUVC){}
  digitalWrite(LED_UVC,LOW);
  rinseAirBusy = false;
}

void dispenseClean1(float volume){
  rinse1Busy = true;
  digitalWrite(RINSE_PUMP1,HIGH);
  dispenseClean1Time = volume * flowClean1;
  //Start Timer
  timeRinseStart1 = millis();
}

void dispenseClean2(float volume){
  rinse2Busy = true;
  digitalWrite(RINSE_PUMP2,HIGH);
  dispenseClean2Time = volume * flowClean2;
  //Start Timer
  timeRinseStart2 = millis();
}

void dispenseAir(){
  rinseAirBusy = true;
  digitalWrite(RINSE_AIRVALVE,HIGH);
  timeRinseAir = millis();
}

void enableUVCLed(){
  cli();//stop interrupts

  //set timer4 interrupt at 1Hz
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR4A = 15624/1;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);

  sei();//allow interrupts
  
  //digitalWrite(LED_UVC,HIGH);
  timeUVCLed = millis();
  rinseUVCBusy = true;
  
}

void disableUVCLed(){
  //Disable timer compare interrupt
  OCR4A = 0;
  TIMSK4 |= (0 << OCIE4A);
  //digitalWrite(LED_UVC,LOW);
  timeUVCLed = 0;
  rinseUVCBusy = false;
}
ISR(TIMER4_COMPA_vect){//timer4 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)

 digitalWrite(LED_UVC,toggleLed);
 toggleLed = !toggleLed;

}

void rinse1OFF(){
  rinse1Busy = false;
  digitalWrite(RINSE_PUMP1,LOW);
  //Clear Timer
  timeRinseStart1 = 0;
}

void rinse2OFF(){
  rinse2Busy = false;
  digitalWrite(RINSE_PUMP2,LOW);
  //Clear Timer
  timeRinseStart2 = 0;
}

void rinseAirOFF(){
  rinseAirBusy = false;
  digitalWrite(RINSE_AIRVALVE,LOW);
  //Clear Timer
  timeRinseAir = 0;
}
