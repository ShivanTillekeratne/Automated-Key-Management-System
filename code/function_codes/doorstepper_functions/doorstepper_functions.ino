//Door motor function
//door up function
void doorUp(){
  stepper.moveTo(16*1500);
  while (stepper.currentPosition() != 16*1500-100)
  stepper.run();
  stepper.stop();
  stepper.runToPosition(); 
}
//door down function
void doorDown(){
  stepper.moveTo(0);
  while (stepper.currentPosition() != +100)
  stepper.run();
  stepper.stop();
  stepper.runToPosition();
}