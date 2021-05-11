#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoOTA.h>


constexpr uint8_t RST_PIN = D3;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;     // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
String tag;
String accessGranted [2] = {"4120097229", "19612012715"};  //RFID serial numbers to grant access to
int accessGrantedSize = 2;                                //The number of serial numbers




// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "QEBSDNb7mt1J-Da5vZRClJkACoVFexTb";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "HUAWEI-2.4G-y9qK";
char pass[] = "3fEh2Qpd";

Servo lockServo;
WidgetLED led4(V5);
BlynkTimer timer;
WidgetTerminal terminal(V0);

int tracking = 0;
int flag = 0;

BLYNK_WRITE(V0)
{
  int pincode = 5646;
  // if you type "Marco" into Terminal Widget - it will respond: "Polo:"
  if (pincode == param.asInt()) {
    terminal.clear();
    terminal.println("Pin Accepted") ;
    flag = 1;
    
  } else {

    // Send it back
    terminal.println("Pin Denied");
    //terminal.write(param.getBuffer(), param.getLength());
    //terminal.println();
    flag = 0;
  }

  // Ensure everything is sent
  terminal.flush();
}




BLYNK_WRITE(V1)
{
  
  Serial.println(param.asInt());

  

  
  
  if(param.asInt() == 1)
  {

    if(flag == 1){
      lockServo.write(0);
      delay(1500);
      lockServo.write(90);
      tracking = 0;
    }

    
    
    
  }
  else if(param.asInt() ==0){
    lockServo.write(180);
    delay(1500);
    lockServo.write(90);
    tracking = 1;
    flag = 0;
   
  }
  
}


void reconnectBlynk() { //connection checker
  if (!Blynk.connected()) {

    Serial.println("Lost connection");
    if(Blynk.connect()) {
      Serial.println("Reconnected");
    }
    else {
      Serial.println("Not reconnected");
    }
  }
}



void setup()
{
  // Debug console
  Serial.begin(9600);
  
  terminal.clear();
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  timer.setInterval(30*1000, reconnectBlynk); //run every 30s
  
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  lockServo.attach(16);


  terminal.print(F("Enter your pin:"));
  terminal.flush();
}

void loop()
{
  //*** run the program if blynk is connected to network ***//
  timer.run(); 
  if(Blynk.connected()) { Blynk.run(); }
  ArduinoOTA.handle(); 

  
  Blynk.virtualWrite(V2, tracking);

  if(tracking == 1){
    Blynk.virtualWrite(V1, 0);
    //the door is lock
    led4.on();

    
  }
  else{
    Blynk.virtualWrite(V1, 1);
    led4.off();
  }
  
  String temp = "";
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }
    Serial.println(tag);
    
    temp = tag;
    checkAccess (tag);

    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(50);
  }
}

void checkAccess (String temp)    //Function to check if an identified tag is registered to allow access
{
  boolean granted = false;
  for (int i=0; i <= (accessGrantedSize-1); i++)    //Runs through all tag ID numbers registered in the array
  {
    if(accessGranted[i] == temp)            //If a tag is found then open/close the lock
    {
      Serial.println ("Access Granted");
      granted = true;
      if (tracking == 1)         //If the lock is closed then open it
      {
          Serial.println("unlock");
          
          
          lockServo.write(0);
          
          delay(1500);
          lockServo.write(90);
          tracking = 0;

          
          
          
      }
      else if (tracking == 0)   //If the lock is open then close it
      {
          Serial.println("lock");
          
          
          lockServo.write(180);
          
          delay(1500);
          lockServo.write(90);
          tracking = 1;

          
      }

    }
  }
  if (granted == false)     //If the tag is not found
  {
    Serial.println ("Access Denied");

  }
}
