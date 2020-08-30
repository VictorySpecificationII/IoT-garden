//Author: Antreas Christofi
//Date: Aug 24th 2020
//License: GNU General License


#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//Create software serial object to communicate with the SIM900
SoftwareSerial SIM900Serial(10,11);//SIM900 Tx ad Rx pins connected to pins 10 and 11

//Set the LCD address to 0x27 for a 16 character, 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Comms section
String msg;
char call;//todo: experiment with String when MCU is online

void setup(){

  //Serial Section
  Serial.begin(19200);
  SIM900Serial.begin(19200);

  //LCD Section
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  
  printToLCD(0, 0, "IoT Hydroponics v0.1...");
  delay(1000);
  printToLCD(1, 1, "Built by: Antreas Christofi");
  delay(1000);
  ScrollText("L");
  clearLCD();

  //Pin Section
  pinMode(13, OUTPUT);//Onboard LED pin for debugging the NetLight of the SIM900
  pinMode(9, OUTPUT);//Software power on
  pinMode(3, OUTPUT);//Reset signal to SIM900
  pinMode(2, OUTPUT);//Relay signal pin
  digitalWrite(2, HIGH);//Disable relay on boot, active low

  SIM900Serial.println("AT+CNMI=2,2,0,0,0\r"); // AT cmd to show output on serial out

  delay(1000);
  printToLCD(0, 0, "Boot-up complete...");
  delay(500);
  ScrollText("L");
    
  clearLCD();
  clearLCD();
  
  }

void loop(){

      ReceiveMessage();
      // if received command is to turn on relay
      if(msg.indexOf("On")>=0)
      {
        EnableRelay();
        // Send a sms back to confirm that the relay is turned on
        SendMessage("Irrigation Started.");
      }
  
      // if received command is to turn off relay
      if(msg.indexOf("Off")>=0)
      {
        DisableRelay();
        // Send a sms back to confirm that the relay is turned off
        SendMessage("Irrigation Stopped.");
  }
     
}//END LOOP


//*********************************************DISPLAY FUNCTIONS*******************************************//

void printToLCD(int col, int row, String toPrint){
  lcd.setCursor(col, row);
  Serial.print(toPrint + "\n");
  lcd.print(toPrint);
  }
  
void ScrollText(String direction){
  if(direction == "L"){
    
    for(int i = 0; i < 28; i++){
      lcd.scrollDisplayLeft();
      delay(200);
      }
      
   }
   else if (direction == "R"){
    
    for(int i = 0; i < 28; i++){
      lcd.scrollDisplayRight();
      delay(200);
      }
      
    }
    
  }
  
void clearLCD(){
  lcd.clear();
  }
  
//**********************************************RELAY FUNCTIONS********************************************//

void EnableRelay(){
  digitalWrite(2, LOW); //Enable Relay, active low
  Serial.print("Log: Relay Enabled.\n");
  //relayState = true;
  delay(1000);
  }

void DisableRelay(){
  digitalWrite(2, HIGH); //Disable Relay, active high
  Serial.print("Log: Relay Disabled.\n");
  //relayState = false;
  delay(1000);
  }

//*********************************************POWER FUNCTIONS*******************************************//

void SoftwareShutDown(){
  SIM900Serial.println("AT+CPOWD=1");
  delay(1000);
  }

void SoftwarePowerOn(){
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  }

//*********************************************GSM/GPRS FUNCTIONS*******************************************//


void UpdateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    SIM900Serial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(SIM900Serial.available()) 
  {
    Serial.write(SIM900Serial.read());//Forward what Software Serial received to Serial Port
  }
}

//*********************************************GSM/DIAG FUNCTIONS*******************************************//
  
void UnlockSIM(){
  SIM900Serial.println("AT+CPIN=1234");
  delay(1000);
  }

void CheckRegistrationStatus(){
  SIM900Serial.println("AT+CREG?");//Ask for registration status
  delay(2500);
  }

//*********************************************GSM/MESG FUNCTIONS*******************************************//
void ReceiveMessage(){

  if (SIM900Serial.available()>0){
    msg=SIM900Serial.readString();
    Serial.print(msg);
  }
}

void SendMessage(String contents)
{
  SIM900Serial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  SIM900Serial.println("AT+CMGS=\"+35797800474\"\r"); // Replace x with mobile number
  delay(1000);
  SIM900Serial.println(contents);// The SMS text you want to send
  delay(100);
   SIM900Serial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

//*********************************************GSM/CALL FUNCTIONS*******************************************//
void MakeCall(){
  SIM900Serial.println("ATD+35797800474;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
  Serial.println("Calling..."); // print response over serial port
  delay(1000);
}

void HangupCall(){
  SIM900Serial.println("ATH");
  Serial.println("Hangup Call...");
  delay(1000);
}

void ReceiveCall(){
  SIM900Serial.println("ATA");
  delay(1000);
  {
    call=SIM900Serial.read();
    Serial.print(call);
  }
}

void RedialCall(){
  SIM900Serial.println("ATDL");
  Serial.println("Redialing....");
  delay(1000);
}
