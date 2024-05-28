//LCD Functions
//display push in or out
void push_lcd(){
    lcd.backlight();
    lcd.setCursor(3,0);
    lcd.print("Push The");
    lcd.setCursor(0,1);
    lcd.print("In or Out Button");
}
//enter the fingerprint
void enter_fingerprint_lcd(){
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Please Enter");
      lcd.setCursor(0,1);
      lcd.print("The Fingerprint");
}
//try again
void tryagain_lcd(){
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Try Again");
}
//enter lecture hall number
void enter_hall_lcd(){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Enter Lecture");
      lcd.setCursor(2,1);
      lcd.print("Hall Number");
}
//invalid hall number
void invalid_hall_lcd(){
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("Invalid");
      lcd.setCursor(2,1);
      lcd.print("Hall Number");
}
//hall key is already taken
void hall_empty_lcd(){
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("The Hall key is");
      lcd.setCursor(1,1);
      lcd.print("already taken");
}
//take the key
void take_lcd(){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Take The Key");  
}
//thank you
void thank_lcd(){
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Thank You!");
      lcd.noBacklight();
}
//key chamber occupied
void not_empty_lcd(){
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("The Hall is");
      lcd.setCursor(0,1);
      lcd.print("Already Occupied");
}
void keepkey_lcd(){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Keep the key");
}