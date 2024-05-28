//Keypad function with lcd
int get_hall_no() { 
  String inputString; 
  int inputInt;
  int col=1; 
  lcd.clear();
  while (true) { 
    char key = kypd.getKey(); 
    if (key) { 
      if (key >= '0' && key <= '9') {   
        inputString += key;   
        lcd.setCursor(col++,0);
        lcd.print(key);         
      } else if (key == '#') { 
        lcd.clear();
        if (inputString.length() > 0) { 
          inputInt = inputString.toInt();   
          inputString = "";// clear input 

          if(inputInt>0 && inputInt<=12){ 
            return inputInt; 
          } 
          return -2; //invalid hall                 
        } 
        return -1; //please enter hall number 
      } else if (key == '*') { 
        lcd.clear();
        inputString = "";
        col=1;  // clear input 
      } 
    } 
  } 
}