#include <HCSR04.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//definire variabile asociate servomotoarelor
 Servo barrierIn; 
 Servo barrierOut; 

//definire variabile asociate senzorilor
 HCSR04 sensorIn(A2,A3);
 HCSR04 sensorOut(A0,A1);

//definire variabilă asociată LCD-ului
 LiquidCrystal_I2C lcd(0x27, 16, 2);    

 int sInValue, sOutValue; //definire variabile pentru valorile citite de la senzori
 int vacantPlaces = 1; //definire variabilă pentru numărul de locuri din parcare
 int hasEntered = 0, hasExited = 0; //definire variabile flag

//definire variabile asociate LED-urilor
 int redLed = 6;
 int greenLed = 7;

void printOnLCD(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Locuri libere");
  lcd.setCursor(7,1);
  lcd.print(vacantPlaces);
}

void barrierIsOpening(){
  lcd.setCursor(4,0);
  lcd.print("Bariera");
  lcd.setCursor(2,1);
  lcd.print("se deschide");
}

void barrierIsClosing(){
  lcd.setCursor(4,0);
  lcd.print("Bariera");
  lcd.setCursor(2,1);
  lcd.print("se inchide");
}

void setup() {
  Serial.begin(115200); //stabilirea comunicației seriale cu placa

  //atașarea pinului 9/10 servomotorului care controlează bariera de la intrare/ieșire
  barrierIn.attach(9);          
  barrierOut.attach(10);        

  //setarea poziției inițiale a barierei de la intrare/ieșire ca fiind de 80/95 de grade
  barrierIn.write(80); 
  barrierOut.write(95); 

  lcd.begin(); //inițializare LCD
  lcd.backlight(); //activare lumină de fundal
  
  //se setează pinii LED-urilor ca OUTPUT
   pinMode(redLed,OUTPUT);
   pinMode(greenLed,OUTPUT);
}

void loop() {
  //citire distanțe senzori
 sInValue=sensorIn.dist();
 sOutValue=sensorOut.dist();

  lcd.begin();        

  //aprindere LED-uri în funcție de numărul de locuri libere
   if(vacantPlaces > 0)
   {
     digitalWrite(greenLed,HIGH);
     digitalWrite(redLed,LOW);
   }
   if(vacantPlaces == 0)
   {
     digitalWrite(redLed,HIGH);
     digitalWrite(greenLed,LOW);
   }
  
  //intrarea în parcare
   if(sInValue >=1 && sInValue <= 5 && vacantPlaces > 0 && hasEntered == 0)
   {
     //ridicarea barierei
    for(int pos=80;pos>=0;pos-=2)
    {
      barrierIsOpening();
      barrierIn.write(pos);
    }
    hasEntered = 1;
    vacantPlaces--;
  }
  if(hasEntered == 1)
  { 
    delay(1000);
   //coborârea barierei
    for(int pos=0;pos<=80;pos+=2)
    {
      barrierIsClosing();
      barrierIn.write(pos);
    }
    hasEntered = 0;
  }

  //ieșirea din parcare
  if(sOutValue >=1 && sOutValue <= 5 && hasExited == 0)
  {
    //ridicarea barierei
    for(int pos=95;pos>=10;pos-=2)
    {
      barrierIsOpening();
      barrierOut.write(pos);
    }
    hasExited = 1;
    vacantPlaces++;
  }
  if(hasExited == 1)
  { 
    delay(1000);
    //coborârea barierei
    for(int pos=10;pos<=95;pos+=2)
    {
      barrierIsClosing();
      barrierOut.write(pos);
    }
    hasExited = 0;
  }

  //asigurare că vacantPlaces nu depășește intervalul [0,10]
  if(vacantPlaces > 10)
  {
    vacantPlaces = 10;
  }
  if(vacantPlaces < 0)
  {
    vacantPlaces = 0;
  }

  //afișare pe LCD
  printOnLCD();
  delay(100);
}