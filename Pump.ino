
/*
//Dispense mL mL of liquid
void dispense(double ml){
//1 full turn dispense about 125 uL 
//there is 8 rollers so the precision of the pump is of about 15 uL
//1 full turn is made of 200*256 = 51200 impulses
  double volPerRev = 0.135;
  double Rev = ml/volPerRev;
  Serial.println(Rev);
  double stepPerRev = 51200;
  uint32_t stepToDo = Rev*stepPerRev;
  uint32_t Xactual = readReg(0x21);
  Xactual = readReg(0x21);
  uint32_t Xtarget = Xactual + stepToDo;
  Serial.print("To dispense ");Serial.print(ml);Serial.print(" mL of water, ");Serial.print(stepToDo);Serial.println(" steps must be made");
  writeReg(0x37,Xtarget);
}
*/
