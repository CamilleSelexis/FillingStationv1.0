//During scale reading the interrupts are disabled so we cannot read the scale while the pump is running in non-blocking mode

void initializeScale1(){
  PRINTLN("Init Scale 1");
  scale1.begin(SCALE1_DOUT, SCALE1_CLK);
  scale1.set_scale(calibration_factor1);
  scale1.set_offset(zero1);
  if (scale1.wait_ready_timeout(1000)) {
    scaleWeight1 = scale1.get_units(10);
    PRINT("Weight: ");
    PRINTLN(scaleWeight1);
    scaleInit1 = true;
  } else {
    PRINTLN("HX711 not found.");
    scaleInit1 = false;
  }
  getScaleState1();
}

void selectScale1(){
  if(scaleInit1 && scaleWeight1 > 0){
    scaleActive1 = true;
    if(scaleActive2)
      scaleActive2 = false;
    }
  else{
    scaleActive1 = false;
  }
}

void tareScale1(){
  scale1.tare();
  zero1 = scale1.read_average(); //Get a baseline reading
  scale1.set_offset(zero1);
}

void setCalibration1(){
  scale1.set_scale(calibration_factor1); //Adjust to this calibration factor
}

void getScaleState1(){
  if(scaleInit1){
    if(scaleActive1){
      scaleState1 = 1;
    }
    else{
      scaleState1 = 2;
    }
  }
  else{
    scaleState1 = 0;
  }
}

void initializeScale2(){
  PRINTLN("Init Scale 2");
  scale2.begin(SCALE2_DOUT, SCALE2_CLK);
  scale2.set_scale(calibration_factor2);
  scale2.set_offset(zero2);
  if (scale2.wait_ready_timeout(1000)) {
    scaleWeight2 = scale2.get_units(10);
    PRINT("Weight: ");
    PRINTLN(scaleWeight2);
    scaleInit2 = true;
  } else {
    PRINTLN("HX711 not found.");
    scaleInit2 = false;
  }
  getScaleState2();
}

void selectScale2(){
  if(scaleInit2 && scaleWeight2 > 0){
    scaleActive2 = true;
    if(scaleActive1)
      scaleActive1 = false;
    }
  else{
    scaleActive2 = false;
  }
}

void tareScale2(){
  scale2.tare();
  zero2 = scale2.read_average(); //Get a baseline reading
  scale2.set_offset(zero2);
}

void setCalibration2(){
  scale2.set_scale(calibration_factor2); //Adjust to this calibration factor
}

void getScaleState2(){
  if(scaleInit2){
    if(scaleActive2){
      scaleState2 = 1;
    }
    else{
      scaleState2 = 2;
    }
  }
  else{
    scaleState2 = 0;
  }
}

void updateScalesValues(){
  if(scaleInit1){
    if(scale1.wait_ready_timeout(1000)){
      scaleWeight1 = scale1.get_units(10);
      //Serial.println("scale1 average = "+String( scale1.read_average()));
    }
    else{
      scaleWeight1 = -1;
    }
  }
  if(scaleInit2){
    if(scale2.wait_ready_timeout(1000))
      scaleWeight2 = scale2.get_units(10);
      //Serial.print("scale2 average = "+ String( scale2.read_average()));
  }
  else{
    scaleWeight2 = -1;
  }
}

int selectScaleAuto(){
  if(scaleInit1 && scaleInit2 && scaleWeight1 > scaleDeadWeight && scaleWeight2 > scaleDeadWeight){
    if(scaleWeight1 < scaleWeight2){
      scaleActive1 = true;
      scaleActive2 = false;
      return 1;
    }
    else{
      scaleActive1 = false;
      scaleActive2 = true;
      return 2;
    }
  }
  if(scaleInit1 && scaleWeight1 > scaleDeadWeight){
    scaleActive1 = true;
    scaleActive2 = false;
    return 1;
  }
  if(scaleInit2 && scaleWeight2 > scaleDeadWeight){
    scaleActive1 = false;
    scaleActive2 = true;
    return 2;
  }
  scaleActive1 = false;
  scaleActive2 = false;
  return 0;
}
