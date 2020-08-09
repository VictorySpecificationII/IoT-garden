#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

//Create software serial object to communicate with the SIM900
SoftwareSerial SIM900Serial(10,11);//SIM900 Tx ad Rx pins connected to pins 7 and 8

//Set the LCD address to 0x27 for a 16 character, 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){
  
  //Serial Section

  Serial.begin(9600);
  
  //LCD Section
  Serial.print("Initializing LCD...\n");
  lcd.init(); 
  lcd.backlight();
  lcd.clear();
  
  printToLCD(0, 0, "IoT Hydroponics v0.0...");
  delay(1000);
  printToLCD(1, 1, "Built by: Antreas Christofi");
  delay(1000);

  for(int i = 0; i < 28; i++){
  lcd.scrollDisplayLeft();
  delay(200);
  }

  clearLCD();

  
  
  //Pin Section
  Serial.print("Initalizing Pin I/O...\n");
  pinMode(2, OUTPUT);//Relay signal pin
  digitalWrite(2, HIGH);//Disable relay on boot, active low
  pinMode(13, OUTPUT);//Onboard LED pin for debugging the NetLight of the SIM900
  pinMode(3, OUTPUT);//Reset signal to SIM900
  

  //SIM900 Section
  Serial.print("Initializing SIM900 Serial...\n");
  SIM900Serial.begin(9600);
  SIM900Serial.println("AT"); //Handshaking with SIM900
  updateSerial();
  SIM900Serial.println("AT+CSQ"); //Signal quality test, value range is 0-31 , 31 is the best
  updateSerial();
  SIM900Serial.println("AT+CCID"); //Read SIM information to confirm whether the SIM is plugged
  updateSerial();
  SIM900Serial.println("AT+CREG?"); //Check whether it has registered in the network


  }


void loop(){

  //enableRelay();
  //disableRelay();
  //delay(5000);
  //clearLCD();
  //updateSerial();
  //rstSIM900();
 
  }


//*********************************************DISPLAY FCTS*******************************************//

void printToLCD(int col, int row, String toPrint){
  lcd.setCursor(col, row);
  Serial.print(toPrint + "\n");
  lcd.print(toPrint);
  }

void clearLCD(){
  lcd.clear();
  }
  
//**********************************************RELAY FCTS********************************************//

void enableRelay(){
  digitalWrite(2, LOW); //Enable Relay, active low
  Serial.print("Log: Relay Enabled.\n");
  delay(1000);
  }

void disableRelay(){
  digitalWrite(2, HIGH); //Disable Relay, active high
  Serial.print("Log: Relay Disabled.\n");
  delay(1000);
  }

  
//*********************************************GSM/GPRS FCTS*******************************************//

void updateSerial()
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

void rstSIM900(){
  digitalWrite(3, HIGH);
  }
