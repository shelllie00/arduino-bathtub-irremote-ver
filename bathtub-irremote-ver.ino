//temperature sensor
#include <OneWire.h>
#include <DallasTemperature.h>
#define DQ_pin 12
OneWire oneWire(DQ_pin);
DallasTemperature sensors(&oneWire);
int temp; //current temperature

//water pump
#define cold 2
#define hot 3

//water level sensor
#define limmit 500 //limmit = the highest water level
int level;

//LCD monitor
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
int ideal,input[2],show[2],stage=1; // show[] for LCD output

//ir remote
#include <IRremote.h>
byte IR_RECEIVE_PIN=7;
bool irremote = true;

void setup(){  
  //temperature sensor setup
  Serial.begin(9600);
  sensors.begin();
  
  //water pump setup
  pinMode(hot,OUTPUT);
  pinMode(cold,OUTPUT);  
  digitalWrite(cold,LOW);
  digitalWrite(cold,LOW);
  
  //lcd monitor setup
  lcd.init();
  lcd.backlight();
  input[0]=-1;
  input[1]=-1;

  //irremote setup
   IrReceiver.begin(IR_RECEIVE_PIN,ENABLE_LED_FEEDBACK);
   delay(100);
}
void loop() 
{
  //remote input
  while(irremote==true){
    if(input[1]==-1){
    lcd.setCursor(0,0);
    lcd.print("Input:");
    if (IrReceiver.decode()){
      if (stage==1){
        convert(0);
      }
      else if (stage>1){
        convert(1);
      }
    } 
   }
   else{
    lcd.clear();
    lcd.setCursor(0,0);
    ideal=input[0]*10+input[1];
    lcd.print("ideal:");
    lcd.print(ideal);
    irremote=false;
   }
   delay(100);
  }
  
  while(level<limmit){
    //current temperature
    sensors.requestTemperatures();
    temp=(sensors.getTempCByIndex(0));
    lcd.setCursor(9,0);
    lcd.print("temp:");
    lcd.print(temp);
    
    //current water level
    level=analogRead(A2);
    lcd.setCursor(0,1);
    lcd.print("level:");
    lcd.print(level);
    
    if (temp<ideal){ //too cold
      digitalWrite(hot,HIGH);
      digitalWrite(cold,LOW);
    }
     
    else if (temp>ideal){ //too hot
      digitalWrite(cold,HIGH);
      digitalWrite(hot,LOW);
    }

    else{ //perfect temperature
      digitalWrite(cold,HIGH);
      digitalWrite(hot,HIGH);
    }
    delay(200);
  } 
  lcd.setCursor(11,1);
  lcd.print("done");
  digitalWrite(cold,LOW);
  digitalWrite(hot,LOW);
 }

 void convert(int x){
  int inf;
  inf=IrReceiver.decodedIRData.command;
  if (inf!=28){ //controller not press "ok"
      switch(inf){
        case 69:
          show[x]=1;
          break;
        case 70:
          show[x]=2;
          break;
        case 71:
          show[x]=3;
          break;
        case 68:
          show[x]=4;
          break;
        case 64:
          show[x]=5;
          break;
        case 67:
          show[x]=6;
          break;
        case 7:
          show[x]=7;
          break;
        case 21:
          show[x]=8;
          break;
        case 9:
          show[x]=9;
          break;
        case 25:
          show[x]=0;
          break;
        default:
          break;
      }
    lcd.setCursor(x,1); //LCD show input num
    lcd.print(show[x]); 
    IrReceiver.resume(); 
  }
  else{ //if controller press "ok"
    stage++;
    input[x]=show[x];   
    IrReceiver.resume(); 
  }
}
