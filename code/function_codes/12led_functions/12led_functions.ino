//12 LED function
void feed(int x) { 
  digitalWrite(clk_led, LOW); 
  digitalWrite(data, x); 
  delay(pulse); 
  digitalWrite(clk_led,HIGH);
}
void push() { 
  digitalWrite(latch_led, LOW); 
  int count = 0; 
  for (int i = 0; i < 16; i++) { 
    if (i < 2 || i == 7 || i == 15) { 
      feed(0); 
      continue; 
    } 
    feed(states[11 - count]); 
    count++; 
  } 
  digitalWrite(latch_led, HIGH); 
}