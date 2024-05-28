//hall effect functions
void clk_hallpulse() { 
  digitalWrite(clk_hall, HIGH); 
  delayMicroseconds(pulse); 
  digitalWrite(clk_hall, LOW); 
}
void readsr() { 
  
  digitalWrite(latch_hall, LOW); 
  digitalWrite(latch_hall, HIGH); 
   
  
  digitalWrite(latch_hall, LOW); 
  delayMicroseconds(50); 
  digitalWrite(latch_hall, HIGH); 
 
  
  for (int i = 0; i < 12; i++) { 
    states[i] = digitalRead(data); 
    clk_hallpulse(); 
  } 
}