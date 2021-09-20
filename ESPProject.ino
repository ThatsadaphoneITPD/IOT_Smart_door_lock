#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

 
 //Declare SS and RST pin on RFID
#define SS_PIN D4
#define RST_PIN D3
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

//declare blynk
#define BLYNK_PRINT Serial
//declare blynk's LCD in App
WidgetLCD lcd(V10);// Create Blynk LCD intance.

// should get Auth Token in the Blynk App.
char auth[] = "KwkQ9d_VOzaqAoL3M2_NZu1mWOgSqwAK";
// Your WiFi credentials.
// Set password to Wifi networks.
//Home WIFI, we must change or set new this ssiad and pass. everytime,if we wave stay in different place and WIFI
char ssid[] = "Diem";
char pass[] = "huynhthidiem";
//University WIFI
//char ssid[] = "Greenwich-Student";
//char pass[] = "12345678";

// value Pin with Led 
int red = D0;
int white = D1;

// declare Servo
Servo servoMotor;
int pos = 0;// decare setup value position of Servo motor with 0"

//declare switch Sensor
const int SwitchSensor = D2;
int state; 
bool isOpen = false;


void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  Blynk.begin(auth, ssid, pass);
  pinMode(red, OUTPUT);// Led D0
  pinMode(white, OUTPUT); // Led D1
  pinMode(SwitchSensor, INPUT_PULLUP);// switch sensor D2
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  servoMotor.attach(D8);//Initiate servo PIn
  servoMotor.write(pos);//Initiate Servo original position
}

void turn_on(int led, int TIME)//funtion turn on The LED 
{
  digitalWrite(led, HIGH);   // light reb (send 5V to the pin)
  delay(TIME);               // as wait 4000ms = 4s
  digitalWrite(led, LOW);
}


// Add funtion Door_open
void Door_open(){
  for (pos = 0; pos <= 90; pos += 11)
    {
      servoMotor.write(pos);
      delay(30);
    }
    Serial.print("Open Door");
     Serial.println("");
      turn_on(white, 200);
  }
  //
// Add Funtion Door_close
 void Door_close(){
  for (pos = 90; pos >= 0; pos -= 11)
    {
      servoMotor.write(pos);
      delay(30);
    }
    Serial.print("Close Door");
     Serial.println("");
      turn_on(red, 200);
  }
//

//Function switch remote on Blynh App's Button
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();

  if(pinValue == 1)
{
   Serial.print("Open");
    Serial.println("");
   lcd.print(1, 0, "OPEN");// LCD print, column 1, row 1 in Blynk LCD
   Door_open(); //use function on remote
  }
  else
{   
    Serial.println("Close");
     Serial.println("");
    lcd.print(1, 0, "CLOSE"); // LCD print, column 1, row 1 in Blynk LCD
    Door_close();//use function on remote
  }
}

//Read Door state on switch sensor
void doorState()
{
   state = digitalRead(SwitchSensor);
  if (state == HIGH) {
    if(!isOpen)
    {
    Serial.println("Open");
    Serial.println(" ; ");
    lcd.print(1, 0, "Open");
    isOpen = true;
    }
  }
  else {
    if(isOpen)
    {
    Serial.println("Close");
     Serial.println(" ; ");
     lcd.print(1,0, "Close");
     isOpen = false;
     Door_close();
    }
  }
  delay(1000);
}


void RFID()
{
// Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  lcd.print(1, 0, content.substring(1));
  //delay(3000);// if put delay here, will delay Unlock trigger before 3s
  //lcd.clear();
  if (content.substring(1) == "59 1E 4B B2") //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Authorized access");
    lcd.print(2, 1, "Access");
    turn_on(white, 600);
    Serial.println();
    delay(400);
    //Unlock by servo
    Door_open();
  }
  else
  {
   Serial.println("not owener");
   lcd.print(2,1, "Welcome Robber");
   turn_on(red, 400);
   servoMotor.write(pos);
  }
  delay(3000);// so we put it here then will dalay clear function Display in Blynk LCD after 3s
  lcd.clear();
}

void loop()
{
  Blynk.run();
  RFID();
  doorState();
  }
